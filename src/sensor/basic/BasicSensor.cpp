#include "sensor/basic/BasicSensor.h"
#include <ArduinoJson.h>
#include "link/link.h"

void BasicSensor::update_value() {
    last_update_time = millis();
}

void BasicSensor::refresh() {
    if (task_handle != nullptr) {
        xTaskNotifyGive(task_handle);
    }
}

void BasicSensor::attach() {
    run_task(); // 启动定时任务
}

void BasicSensor::detach() {
    if (task_handle != nullptr) {
        TaskHandle_t handle = task_handle;
        task_handle = nullptr;
        vTaskDelete(handle);
    }
    last_update_time = 0;
}

void BasicSensor::update_to_client() {
    if (!WsIsLink()) return;
    JsonDocument json;
    json["type"] = "sensor-update";
    json["sensorId"] = id;
    JsonArray value_array = json["values"].to<JsonArray>();
    for (const auto &item: values) {
        JsonObject value_json = value_array.add<JsonObject>();
        value_json["n"] = item.name;
        value_json["u"] = item.unit;
        value_json["v"] = item.value;
    }

    WsAutoSend(json);
}


// 定时任务
void BasicSensor::task_runner(void *parameter) {
    auto *sensor = static_cast<BasicSensor *>(parameter);
    while (sensor->task_handle != nullptr) {
        TickType_t wait_ticks = portMAX_DELAY;
        if (sensor->update_time != 0) {
            wait_ticks = pdMS_TO_TICKS(sensor->update_time);
        }
        ulTaskNotifyTake(pdTRUE, wait_ticks);
        sensor->update_value();
        sensor->update_to_client();
    }
    vTaskDelete(nullptr);
}


void BasicSensor::run_task() {
        if (task_handle == nullptr) {
        xTaskCreatePinnedToCore(task_runner, nullptr, 4096, this, 1, &task_handle, 0);
        if (task_handle != nullptr) {
            xTaskNotifyGive(task_handle);
        }
    }
}
