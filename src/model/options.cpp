// 管理一些配置
#include "model/options.h"
#include "walk/walk.h"
#include "sensor/sensor.h"
#include <model/oled.h>
#include <main.h>
#include <memory>
#include <Preferences.h>

Preferences prefs;
JsonDocument *OptionsHandle(JsonDocument &data){
    String mode = data["mode"];
    const char *key = data["key"];
    auto *req = new JsonDocument();
    JsonDocument &json = *req;
    if(mode == "get"){
        json["data"] = OptionsGet(key);
    }else if(mode == "set"){
        OptionsSet(key, data["data"]);
    }else if(mode == "remove"){
        OptionsRemove(key);
    }
    return req;
}

void update(const char *key){
    if(String(key) == OPTIONS_WALK_EXTENDS){
        WalkInit();
    }
    if(String(key) == OPTIONS_WALK_PWMS){
        WalkInit();
    }
    if(String(key) == OPTIONS_WALK_DIGITALS){
        WalkInit();
    }
    if(String(key) == OPTIONS_WALK_SERVOS){
        WalkInit();
    }
    if(String(key) == OPTIONS_WALK_GROUPS){
        WalkInit();
    }
    if(String(key) == OPTIONS_SCREEN){
        OledReset();
    }
    if(String(key) == OPTIONS_SENSORS){
        SensorInit();
    }
}

void OptionsRemove(const char *key) {
    prefs.begin("options");
    if(prefs.isKey(key)){
        prefs.remove(key);
    }
    prefs.end();
    update(key);
}

void OptionsSet(const char *key, const char *value) {
    prefs.begin("options");
    prefs.putString(key, value);
    prefs.end();
    update(key);
}

String OptionsGet(const char *key) {
    // 板载阿达io配置，只读
    if(String(key) == OPTIONS_BUILT_IN_IO){
        return OPTIONS_BUILT_IN_IO_DEK;
    }
    prefs.begin("options", true);
    if(!prefs.isKey(key)){ // 空配置
        prefs.end();
        // 默认的扩展配置
        if(String(key) == OPTIONS_WALK_EXTENDS){
            return OPTIONS_WALK_EXTENDS_DEF;
        }
        // 自带的组合
        if(String(key) == OPTIONS_WALK_GROUPS){
            return OPTIONS_WALK_GROUPS_DEF;
        }
        // 自带的PWM输出
        if(String(key) == OPTIONS_WALK_PWMS){
            return OPTIONS_WALK_PWMS_DEF;
        }
        // 自带的输出
        if(String(key) == OPTIONS_WALK_DIGITALS){
            return OPTIONS_WALK_DIGITALS_DEF;
        }
        // 自带的舵机
        if(String(key) == OPTIONS_WALK_SERVOS){
            return OPTIONS_WALK_SERVOS_DEF;
        }
        // 自带的底盘配置
        if(String(key) == OPTIONS_CHASSIS){
            return OPTIONS_CHASSIS_DEF;
        }
        // 自带的云台配置
        if(String(key) == OPTIONS_PZT){
            return OPTIONS_PZT_DEF;
        }
        // 自带的底盘配置
        if(String(key) == OPTIONS_SCREEN){
            return OPTIONS_SCREEN_DEF;
        }
        // 自带的传感器配置
        if(String(key) == OPTIONS_SENSORS){
            return OPTIONS_SENSORS_DEF;
        }
        return "";
    }
    String data = prefs.getString(key, "");
//    data.replace()
    prefs.end();
    return data;
}

bool OptionsGetJson(JsonDocument &json, const char *key) {
    String data = OptionsGet(key);
    if(data != ""){
        DeserializationError error = deserializeJson(json, data);
        if(error) return false;
        return true;
    }
    return false;
}

std::unique_ptr<JsonDocument> OptionsGetJson(const char *key) {
    std::unique_ptr<JsonDocument> json(new JsonDocument()); // 智能指针，防止内存泄漏
    OptionsGetJson(*json, key);
    return json;
}