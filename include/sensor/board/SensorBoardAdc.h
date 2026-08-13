#ifndef BULLM_REMOTE_ESP_ADCSENSOR_H
#define BULLM_REMOTE_ESP_ADCSENSOR_H

#include "sensor/basic/BasicSensor.h"

#define ADC_ATTENUATION ADC_2_5db  // 设置 ADC 衰减，适用于 0 - 3.3V 的输入范围
#define ADC_RESOLUTION 12         // 设置 ADC 分辨率，ESP32-C3 支持最高 12 位

#define ADC_R1          100
#define ADC_R2          10
#define ADC_OFFSET      0 // adb偏移

class SensorBoardAdc : public BasicSensor {
public:
    explicit SensorBoardAdc(uint8_t adc_pin);
    ~SensorBoardAdc() override;

    void attach() override;
    void detach() override;

    void update_value() override;
    
    double offset = 0;

private:
    uint8_t pin = 0;
    uint32_t read_voltage_mv() const;
};

#endif //BULLM_REMOTE_ESP_ADCSENSOR_H
