
#ifndef BULLM_REMOTE_ESP_BASICSENSOR_H
#define BULLM_REMOTE_ESP_BASICSENSOR_H
#include <Arduino.h>
#include <vector>

struct SensorValue {
    String name;
    String unit;
    double value = 0;
};

class BasicSensor {
public:
    BasicSensor() = default;
    virtual ~BasicSensor() = default;
    String id;
    String type;
    std::vector<SensorValue> values; // 多值数据
    unsigned long update_time = 0; // 循环更新时间
    unsigned long last_update_time = 0;

    virtual void refresh(); // 主动刷新
    virtual void attach(); // 连接
    virtual void detach(); // 分离

    virtual void update_value(); // 更新传感器值，同步执行
    void update_to_client(); // 发送数据到客户端

    void run_task(); // 运行任务

private:
    TaskHandle_t task_handle = nullptr;
    static void task_runner(void *parameter);
};

#endif //BULLM_REMOTE_ESP_BASICSENSOR_H
