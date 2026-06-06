#ifndef BULLM_REMOTE_ESP_ADCSENSOR_H
#define BULLM_REMOTE_ESP_ADCSENSOR_H

#include "sensor/basic/BasicSensor.h"

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
