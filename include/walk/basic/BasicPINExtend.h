#include <Arduino.h>
#include <map>

#ifndef ESP32S3_BasicPINExtend_H
#define ESP32S3_BasicPINExtend_H

class BasicPINExtend {
public:
    BasicPINExtend() = default;
    virtual ~BasicPINExtend() = default;
    String id;
    std::map<String, uint8_t> id_to_pin; // 字符串id对应实际的pin // TODO 待改进
    virtual void attach() {}; // 初始化
    virtual void detach() {}; // 脱离
    virtual void reset() {}; // 重置状态
};

#endif //ESP32S3_BasicPINExtend_H
