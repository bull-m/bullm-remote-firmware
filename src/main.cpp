/**
 * 版权所有 © 2025 牛明工作室 / yy祝。保留所有权利。
 * SPDX-License-Identifier: MIT
 * 根据 MIT 许可证（MIT License）授权。
 */
#include <WiFi.h>
#include <Preferences.h>
#include "main.h"
#include <esp_system.h>
#include <nvs_flash.h>
#include "model/adc.h"
#include "model/rgb.h"
#include "model/camera.h"
#include "walk/walk.h"
#include "model/oled.h"
#include "model/i2c.h"
#include "model/wifi.h"
#include "link/link.h"


void reset() {
#ifdef PIN_RESET
    // 重置逻辑
    //将引脚1设置为输入上拉模式
    pinMode(PIN_RESET, INPUT_PULLUP);
    int i = 0;
    rgb_loop_one(0xf3cc0c);
    while (digitalRead(PIN_RESET) == LOW) {
        if (i >= 20) { // 10秒内一直按下复位按钮则复位
            OledText("reseting...");
            rgb_loop_one(0x2ecc71);
            for (size_t j = 0; j < 30; j++) {
                digitalWrite(PIN_TIP_LED, i % 2);
                delay(150);
            }
            Preferences prefs;
            prefs.begin("options");
            prefs.clear();
            prefs.end();
            rgb_color_all(0x1abc9c);
            OledText("reset success !");
            delay(500);
            ESP.restart();
            return;
        }
        OledText("reset by " + String(10 - i / 2) + "s");
        delay(500);
        digitalWrite(PIN_TIP_LED, i % 2);
        i++;
    }
#endif
}

void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    // 指示灯
#ifdef PIN_TIP_LED
    pinMode(PIN_TIP_LED, OUTPUT);
    digitalWrite(PIN_TIP_LED, HIGH);
#endif
    // 电机驱动
#ifdef PIN_MOTOR_ENABLE
    pinMode(PIN_MOTOR_ENABLE, OUTPUT);
    digitalWrite(PIN_MOTOR_ENABLE, LOW);
#endif
    // CAM_3V3 新版硬件都没有这个，使用PWDN引脚关闭摄像头电源
#ifdef PIN_CAM_ENABLE
    pinMode(PIN_CAM_ENABLE, OUTPUT);
    digitalWrite(PIN_CAM_ENABLE, LOW);
#endif

    esp_err_t ret;

    ret = nvs_flash_init(); /* 初始化NVS */
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    I2cInit(); // 初始化i2c
    OledInit(); // 初始化oled
//    RgbInit(); // 初始化rgb
    OledText("-> start");

    reset(); // 复位逻辑

    rgb_loop_one(0x008000);
    OledText("link wifi");
    WifiInit(); // 初始化wifi
    if (WiFiClass::getMode() == WIFI_AP) {
        OledText("wifi: AP Mode");
        OledText("ip: " + WiFi.softAPIP().toString());
    } else {
        OledText("wifi: " + WiFi.SSID());
        OledText("ip: " + WiFi.localIP().toString());
    }
    // 初始化udp服务
    UdpInit();
    // 初始化http的处理
    HttpInit();
    // 启动相机任务
    CameraInit();
    // 初始化adc任务
    AdcInit();
    // 初始化运动
    WalkInit();

    // 关闭指示灯
    digitalWrite(PIN_TIP_LED, LOW);
    OledText("success !");
    delay(1000);

    esp_reset_reason_t reason = esp_reset_reason();
    if (reason != ESP_RST_POWERON) {
        OledText("error restart");
        delay(3000);
    }
    rgb_off();
    OledSetInfoMode(false);
    Serial.println("\n=== 初始化完成 ===");
}

void loop() {
    ws.cleanupClients(1); // 只允许一个
    WifiCheckStatus(); // 定时检查wifi状态
    HttpLoop();
    WalkCheckTimeout();
    // HandleLoop();
    UdpLoop();
}