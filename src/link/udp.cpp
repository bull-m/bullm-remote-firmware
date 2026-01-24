
#include <WiFi.h>
#include <lwip/netdb.h>
#include "link/link.h"
#include "main.h"
#include "model/options.h"

NMWiFiUDP udp;

IPAddress udp_serve_ip;
int udp_serve_port;

void UdpInit() {
    udp.begin(UDP_PORT);
}

uint8_t incomingPacket[537];
uint16_t seq = 0; // 序列号

void UdpLoop() {
    if (udp.parsePacket()) {
        int len = udp.read(incomingPacket, 536);
        if (len > 0) {
//            incomingPacket[len] = 0;
//            for (int i = 0; i < len; i++) {
//                Serial.print(incomingPacket[i]);
//                Serial.print(" ");
//            }
//            Serial.println("");
            // 验证长度
            uint16_t data_len = (incomingPacket[len - 3] << 8) | incomingPacket[len - 2];
            if (data_len != len - 5) {
                return;
            }

            // 验证序列号，确保数据是按顺序到达的
//            uint16_t next_seq = (incomingPacket[len - 5] << 8) | incomingPacket[len - 4];
//            if (next_seq <= seq) { // 接受大于当前seq的序列号
//                // 允许回绕情况：当前seq接近最大值，而next_seq从0开始
//                if (!(seq >= 59900 || next_seq < 100)) {
//                    Serial.println("序列号错误");
//                    return; // 丢弃
//                }
//            }

            // 校验码
            uint8_t check = incomingPacket[len - 1];
            // 校验码验证
            uint8_t calcCheck = 0;
            // 校验码校验的数据范围为0 ~ (len - 1) （即不包括校验码本身）
            for (int i = 0; i < len - 1; i++) {
                calcCheck += incomingPacket[i];
            }
            if (calcCheck != check) {
//                Serial.println("校验码错误");
                return;
            }
            // 更新序列号
//            seq = next_seq;
            // 处理数据
            HandleUdpByte(incomingPacket, data_len);
        }
    }
}

// 服务器认证
void UdpServerAuth() {
    // 认证UDP
    auto network_json = OptionsGetJson(OPTIONS_NETWORK);
    auto network = network_json->as<JsonObject>();
    String host = network["ip"].as<String>();

    // 检查host是否有效
    if (host.isEmpty()) {
        Serial.println("Error: Host is empty");
        return;
    }

    // 解析IP和端口
    String ip;
    uint16_t port = 0;

    int colonIndex = host.indexOf(":");
    if (colonIndex > 0) {
        // 存在端口号
        ip = host.substring(0, colonIndex);
        String portStr = host.substring(colonIndex + 1);
        port = portStr.toInt();
    } else {
        // 不存在端口号，使用默认端口或返回错误
        ip = host;
        port = 80; // 默认端口，可根据实际需求修改
        Serial.println("Warning: Port not specified, using default port 80");
    }

    // 验证端口有效性
    if (port == 0) {
        Serial.println("Error: Invalid port number");
        return;
    }

    // 验证IP地址有效性
    if (ip.isEmpty()) {
        Serial.println("Error: Invalid IP address");
        return;
    }

    udp.beginPacket(ip.c_str(), port);
    JsonDocument json;
    json["type"] = "car";  // 小车认证
    json["token"] = network["token"].as<String>();  // token
    json["mac"] = WiFi.macAddress();  // mac
    String jsonString = "";
    serializeJson(json, jsonString);
    udp.print(jsonString);
    udp.endPacket();
    // 保存UDP目标信息
    auto pHostent = gethostbyname(ip.c_str());
    udp_serve_ip = IPAddress((const uint8_t *) (pHostent->h_addr_list[0]));
    udp_serve_port = port;

    // 输出调试信息
    Serial.print("UDP Auth sent to ");
    Serial.print(ip);
    Serial.print(":");
    Serial.println(port);
    Serial.print(":");
    Serial.println(udp_serve_ip.toString());
}


bool UdpSend(uint8_t type, const uint8_t *data, size_t len) {
    return UdpSend(type, data, len, udp_serve_ip, udp_serve_port);
}

bool UdpSend(uint8_t type, const uint8_t *data, size_t len, const IPAddress &ip, uint16_t port) {
    if (port == 0 || !data || len == 0) return false;
    bool err = false;
    udp.beginPacket(ip, port);
    udp.write(0xaa);
    udp.write(0x66);
    udp.write(0xaa);
    udp.write((len >> 16) & 0xFF);
    udp.write((len >> 8) & 0xFF);
    udp.write(len & 0xff);
    udp.write(type);    // 类型
    uint8_t sum = type; // 初始值为类型字节
    for (size_t i = 0; i < len; i++) {
        uint8_t byte = data[i];
        if (udp.write(byte) == 0) {
            err = true;
            break; // 失败了，取消后续的数据发送
        }
        sum = (sum + byte) & 0xff;
    }
    if (err) {
        udp.stop();
        vTaskDelay(100 / portTICK_PERIOD_MS); // 延时0.1秒
        return false;
    }
    udp.write(sum); // 校验位
    udp.write(0x66);
    udp.write(0xaa);
    udp.write(0x66);
    udp.endPacket();
    return true;
}