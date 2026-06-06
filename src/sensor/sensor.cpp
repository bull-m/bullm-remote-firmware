#include "sensor/sensor.h"
#include "sensor/board/SensorBoardAdc.h"
#include "walk/walk.h"

std::map<String, BasicSensor *> sensors;

static BasicSensor *create_sensor(const JsonObjectConst &config) {
    String type = config["type"] | "";
    if (type == "board-adc") {
        uint8_t pin = config["pin"].as<uint8_t>();
        auto *sensor = new SensorBoardAdc(pin);
        sensor->offset  = config["offset"].as<double>();
        return sensor;
    }
    return new BasicSensor();
}

void detach_all_sensors() {
    for (auto &entry: sensors) {
        auto &item = entry.second;
        if (item != nullptr) {
            item->detach();
            delete item;
            item = nullptr;
        }
    }
    sensors.clear();
}

void SensorInit() {
    detach_all_sensors(); // 先按顺序清空所有对象，防止指针错误

    auto sensors_json = OptionsGetJson(OPTIONS_SENSORS);
    if (!sensors_json || !sensors_json->is<JsonArray>()) return;

    for (JsonObject v: sensors_json->as<JsonArray>()) {
        String id = v["id"];
        sensors[id] = create_sensor(v);
        sensors[id]->id = id;
        sensors[id]->type = v["type"].as<String>();
        sensors[id]->update_time = v["u_t"] | 0;

        sensors[id]->attach();
    }
}

JsonDocument *SensorHandle(JsonDocument &data) {
    String mode = data["mode"];
    if (mode == "refresh") {
        String id = data["id"].as<String>();
        if (sensors[id] != nullptr) {
            sensors[id]->refresh();
        }
    }
    return nullptr;
}
