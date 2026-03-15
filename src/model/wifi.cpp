#include <WiFi.h>
#include "model/options.h"
#include "model/wifi.h"
#include "model/oled.h"
#include <WiFiMulti.h>

WiFiMulti wifiMulti;

String startWifiName = ""; // 首次连接的wifi名称

uint8_t connectMultiWiFi();

void WifiInit() {
    String hostname = "BULLM-REMOTE-" + String(WiFi.macAddress());
    WiFiClass::setHostname(hostname.c_str());
    WiFi.setTxPower(WIFI_POWER_19_5dBm); // 设置发射功率
    auto network_json = OptionsGetJson(OPTIONS_WIFi);
    auto network = network_json->as<JsonObject>();
    auto list = network["list"].as<JsonArray>();
    if (list.size() > 0) {
        for (const auto &item: list) {
            wifiMulti.addAP(item["name"], item["pass"]);
        }
    }
    connectMultiWiFi(); // 连接WiFi
    Serial.println("");
    Serial.print("Z-Car Ready! Ip ");
    Serial.println(WiFiClass::getMode() == WIFI_AP ? WiFi.softAPIP() : WiFi.localIP());
    if(WiFiClass::getMode() != WIFI_AP){
        startWifiName = WiFi.SSID();
    }
}

uint8_t connectMultiWiFi() {
    auto network_json = OptionsGetJson(OPTIONS_WIFi);
    auto network = network_json->as<JsonObject>();
    auto list = network["list"].as<JsonArray>();
    uint8_t status = 0;
    if (list.size() > 0) {
        int i = 0;
        status = wifiMulti.run();
        while ((i++ < 20) && (status != WL_CONNECTED)) {
            if (WiFiClass::status() == WL_CONNECTED)
                break;
            else
                delay(500L);
        }
    }
    if (status != WL_CONNECTED) {
        if(startWifiName != ""){ // 已经连接过一次，并且上次已经成功连接到wifi
            OledText("WiFi Error.");
            delay(500L);
            return -1; // 进入下一轮循环
        }
        // 启动 AP 模式
        Serial.println("WiFi连接失败，启动 AP 模式");
        WiFiClass::mode(WIFI_AP);
        String token = OptionsGet(OPTIONS_TOKEN); // 获取存储的token
        String pass = token == "" ? "bullm1211" : token;
        while(pass.length() < 8){
            pass += "0"; // 补0
        }
        String name = "NM-CAR-" + String(WiFi.macAddress());
        Serial.print("WiFi名称：");
        Serial.println(name);
        Serial.print("WiFi密码：");
        Serial.println(pass);
        WiFi.softAP(name, pass);
        IPAddress myIP(1, 2, 1, 1);
        WiFi.softAPConfig(myIP, myIP, IPAddress(255, 255, 255, 0));
        WiFi.softAPsetHostname("bullm-remote"); // 设置主机
        return 0;
    }
    return 1;
}


void WifiCheckStatus() {
    static ulong timeout = 0;
    static ulong current_millis;
    current_millis = millis();
    if ((current_millis > timeout) || (timeout == 0)) {
        if ((WiFiClass::status() != WL_CONNECTED) && (WiFiClass::getMode() != WIFI_AP)) {
            Serial.println("\nWiFi lost. Call connectMultiWiFi in loop");
            OledSetInfoMode(true);
            OledText("WiFi lost");
            OledText("connecting");
            connectMultiWiFi();
            if(WiFiClass::status() == WL_CONNECTED){
                OledSetInfoMode(false);
            }
        }
        timeout = current_millis + 1000L;
    }
}

JsonDocument *WifiHandle(JsonDocument &data) {
    String mode = data["mode"];
    auto *req = new JsonDocument();
    auto &json = *req;
    if (mode == "scan") {
        int n = WiFi.scanComplete();
        if (n == -2) {
            WiFi.scanNetworks(true);
            json["loading"] = true;
        } else if (n == -1) {
            json["loading"] = true;
        } else if (n) {
            JsonArray arr = json["data"].to<JsonArray>();
            for (int i = 0; i < n; ++i) {
                JsonDocument item;
                item["bssid"] = WiFi.BSSIDstr(i);
                item["ssid"] = WiFi.SSID(i);
                item["rssi"] = WiFi.RSSI(i);
                item["enc"] = WiFi.encryptionType(i);
                item["channel"] = WiFi.channel(i);
                arr.add(item);
            }
            WiFi.scanDelete();
        }
    }
    return req;
}