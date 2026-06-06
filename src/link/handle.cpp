/**
 * @brief 处理数据和发送数据
 */
#include "AsyncWebSocket.h"
#include <Preferences.h>
#include "link/handle.h"
#include "model/rgb.h"
#include "model/camera.h"
#include "model/options.h"
#include "model/wifi.h"
#include "walk/walk.h"
#include "model/i2c.h"
#include "model/info.h"
#include "link/link.h"
#include "main.h"
#include "model/error.h"
#include "sensor/sensor.h"

JsonDocument doc; // 用于接收json

void HandleUdpByte(uint8_t *data, size_t len) {
    int mode = data[0];
    if (mode == 0xF1) {
        uint8_t *arr;
        arr = (uint8_t *) malloc(4 + 6);
        Serial.print("来自局域网扫描: ");
        Serial.print(udp.remoteIP().toString());
        Serial.print(":");
        Serial.println(udp.remotePort());
        // 获取ip地址
        IPAddress ip = WiFi.localIP();
        // 将ip地址复制到arr数组中
        for (int i = 0; i < 4; i++) {
            arr[i] = ip[i];
        }
        // 获取MAC地址
        uint8_t mac[6];
        WiFi.macAddress(mac);
        for (int i = 0; i < 6; i++) {
            arr[4 + i] = mac[i];
        }
        UdpSend(0xF1, arr, 10, udp.remoteIP(), udp.remotePort());
        return;
    };
    HandleWsByte(data, len);
}

void HandleWsByte(uint8_t *data, size_t len) {
    int mode = data[0];
    if (mode == 0xFE) {
        char arr[1] = {0xFE};
        WsAutoSend(arr, 1);
        return;
    }
    WalkHandle(data, len);
}

void HandleWsText(uint8_t *data, size_t len) {
    deserializeJson(doc, data);
    if (doc.overflowed()) {
        WsAutoSend("{\"type\":\"msg\",\"msg\":\"消息长度超出的可接受的长度\"}");
        doc.clear();
        return;
    }
//    Serial.println((char *) data);
    String type = doc["type"];
    JsonDocument *req = nullptr;
    if (type == "sensor")
        req = SensorHandle(doc);
    else if (type == "rgb") // rgb
        req = RgbHandle(doc);
    else if (type == "options") // 配置
        req = OptionsHandle(doc);
    else if (type == "camera") // 相机
        req = CameraHandle(doc);
    else if (type == "wifi") // wifi
        req = WifiHandle(doc);
    else if (type == "walk") // 运动
        req = WalkHandle(doc);
    else if (type == "i2c") // i2c
        req = I2cHandle(doc);
    else if (type == "info") // 信息
        req = InfoHandle(doc);
    if (req != nullptr) {
        auto &json = *req;
        json["i"] = doc["i"]; // 增加i属性
        WsAutoSend(*req);
        delete req; // 释放内存（重要）
    }
    doc.clear();
}


// 断开处理
void HandleDisconnect() {
    digitalWrite(PIN_TIP_LED, LOW); // 指示灯
#ifdef PIN_MOTOR_ENABLE
    digitalWrite(PIN_MOTOR_ENABLE, LOW); // 禁用电机
#endif
    WalkReset();
    CameraStop();
}
// 连接处理
void HandleConnect() {
    WalkReset(); // 重置运动状态
    digitalWrite(PIN_TIP_LED, HIGH); // 指示灯
#ifdef PIN_MOTOR_ENABLE
    digitalWrite(PIN_MOTOR_ENABLE, HIGH); // 启用电机
#endif
    ErrorSendToClient(); // 发送累计的错误信息
}