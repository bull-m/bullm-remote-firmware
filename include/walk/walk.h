#include <ArduinoJson.h>
#include <map>
#include "walk/basic/BasicPINExtend.h"
#include "model/options.h"
#include "walk/basic/BasicOutput.h"


#define CONTROL_BYTE1 0x10
#define CONTROL_BYTE2 0x11
#define CONTROL_BYTES 0x13
#define CONTROL_MAINTAIN 0x12

// 设备的前缀
#define PREFIX_PWM "p:"
#define PREFIX_SERVO "s:"
#define PREFIX_INPUT "i:"
#define PREFIX_DIGITAL "o:"
#define PREFIX_GROUP "g:"

// pin的前缀
#define PREFIX_EXTEND "e:"
#define PREFIX_PIN "io:"

extern std::map<String, BasicPINExtend *> extends;
extern std::map<uint16_t, BasicOutput *> outputs;

void WalkHandle(uint8_t *data, size_t len);
JsonDocument *WalkHandle(JsonDocument &data);

void WalkInit();
void WalkReset();
void WalkCheckTimeout();

void InitExtend();
void ExtendDetach();
void ExtendReset();

void InitPwm();
void InitServo();
void InitGroup();
void InitDigital();
void GroupItemInit(JsonObject *_item, int key);

// 计算key, 和前端代码同步，不允许随便修改
namespace RemoteKey {
    // 自定义组合
    int group(uint16_t index);
    // 自定义舵机
    int servo(uint16_t index);
    // 自定义pwm
    int pwm(uint16_t index);
    // 自定义输出
    int digital(uint16_t index);
    // 扩展板里面定义的组合
    int extendDrive(uint16_t index);
}
namespace DeviceId {
    String extendGroup(const String& extend_id, uint8_t pin);
    String extendServo(const String& extend_id, uint8_t pin);
    String extendPwm(const String& extend_id, uint8_t pin);
    String extendDigital(const String& extend_id, uint8_t pin);
}

// 生成引脚完整id
namespace PinId {
    // 扩展引脚
    String extend(const String& extend_id, uint8_t pin);
    // 普通引脚
    String pin(uint8_t pin);
}

// 根据id获取PWM对象
BasicOutput *getOutputById(const String &id);
// 根据id获取PWM对象
BasicPINExtend *getExtendById(const String &id);