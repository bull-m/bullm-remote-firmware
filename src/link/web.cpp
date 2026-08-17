#include "link/link.h"
#include "model/options.h"
#include "index_html.h"
#include <Update.h>

void WebInit() {
    server.on("/ping", HTTP_GET, [](AsyncWebServerRequest *request) {
        JsonDocument json;
        json["ip"] = WiFi.localIP().toString();
        json["mac"] = WiFi.macAddress();
        json["type"] = INFO_TYPE;
        String jsonString = "";
        serializeJson(json, jsonString);
        request->send(200, "application/json", jsonString);
        json.clear();
    });
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/html", index_html);
    });
    server.on("/api/info", HTTP_GET, [](AsyncWebServerRequest *request) {
        JsonDocument json;
        json["ip"] = WiFi.localIP().toString();
        json["mac"] = WiFi.macAddress();
        json["type"] = INFO_TYPE;
        json["version"] = INFO_VERSION;
        json["hardware_version"] = INFO_HARDWARE_VERSION;
        json["esp_reset_reason"] = esp_reset_reason();
        String jsonString = "";
        serializeJson(json, jsonString);
        request->send(200, "application/json", jsonString);
        json.clear();
    });
    // 获取wifi
    server.on("/api/wifi", HTTP_GET, [](AsyncWebServerRequest *request) {
        String data = OptionsGet(OPTIONS_WIFi);
        if (data == "") {
            data = "{}";
        }
        request->send(200, "application/json", data);
    });
    // 设置wifi
    server.on("/api/wifi-set", HTTP_GET, [](AsyncWebServerRequest *request) {
        String data = request->getParam("data")->value();
        if (data != "") {
            OptionsSet(OPTIONS_WIFi, data.c_str());
            request->send(200, "application/json", "{\"code\":200}");
        } else {
            request->send(501, "text/plain", "error");
        }
    });
    //First request will return 0 results unless you start scan from somewhere else (loop/setup)
    //Do not request more often than 3-5 seconds
    server.on("/api/scan", HTTP_GET, [](AsyncWebServerRequest *request) {
        String json = "[";
        int n = WiFi.scanComplete();
        if (n == -2) {
            WiFi.scanNetworks(true);
        } else if (n) {
            for (int i = 0; i < n; ++i) {
                if (i) json += ",";
                json += "{";
                json += "\"rssi\":" + String(WiFi.RSSI(i));
                json += ",\"ssid\":\"" + WiFi.SSID(i) + "\"";
                json += ",\"bssid\":\"" + WiFi.BSSIDstr(i) + "\"";
                json += ",\"channel\":" + String(WiFi.channel(i));
                json += ",\"secure\":" + String(WiFi.encryptionType(i));
                json += "}";
            }
            WiFi.scanDelete();
            if (WiFi.scanComplete() == -2) {
                WiFi.scanNetworks(true);
            }
        }
        json += "]";
        request->send(200, "application/json", json);
    });
    // 重启设备
    server.on("/api/system/restart", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "application/json", "{\"code\":200}");
        // 使用任务延迟重启，确保响应能够发送
        xTaskCreate([](void *param) {
            vTaskDelay(pdMS_TO_TICKS(500)); // 延迟500ms
            ESP.restart();
            vTaskDelete(NULL);
        }, "restart_task", 2048, NULL, 1, NULL);
    });

    server.on("/api/ota", HTTP_POST, [](AsyncWebServerRequest *request) {
      if (request->getResponse()) {
        return;
      }
      // 未上传任何内容 ?
      if (!request->_tempObject) {
        return request->send(400, "text/plain", "未上传任何内容");
      }
      request->send(200, "text/plain", "OK");
    }, [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
        Serial.printf("Upload[%s]: start=%u, len=%u, final=%d\n", filename.c_str(), index, len, final);
        if (!index) {
            Serial.println("OTA update started, file: " + filename);
            if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
                Update.printError(Serial);
                request->send(500, "text/plain", "OTA初始化失败");
                return;
            }
        }

        if (len) {
            if (Update.write(data, len) != len) {
                Update.printError(Serial);
                Update.end(false);
                request->send(400, "text/plain", "更新写入失败");
                return;
            }
        }

        if (final) {
            if (!Update.end(true)) {
                Update.printError(Serial);
                request->send(400, "text/plain", "更新失败");
                return;
            }
            request->send(200, "text/plain", "OTA成功，即将重启设备");
            Serial.println("OTA update successful");
            xTaskCreate([](void *param) {
                vTaskDelay(pdMS_TO_TICKS(2000));
                ESP.restart();
                vTaskDelete(NULL);
            }, "ota_restart_task", 2048, NULL, 1, NULL);
        }
    });
}