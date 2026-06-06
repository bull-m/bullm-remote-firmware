#include "../../../include/sensor/board/SensorBoardAdc.h"
#include <esp32-hal.h>
#include "model/options.h"
SensorBoardAdc::SensorBoardAdc(uint8_t adc_pin) : pin(adc_pin) {
    type = "board-adc";
}

SensorBoardAdc::~SensorBoardAdc() {
    SensorBoardAdc::detach();
}

void SensorBoardAdc::attach() {
    pinMode(pin, INPUT);
    analogReadResolution(ADC_RESOLUTION);
    analogSetPinAttenuation(pin, ADC_ATTENUATION);
    BasicSensor::attach();
}

void SensorBoardAdc::detach() {
    BasicSensor::detach();
}


uint32_t SensorBoardAdc::read_voltage_mv() const {
    constexpr int sampling_frequency = 300;
    uint32_t sum = 0;
    for (int i = 0; i < sampling_frequency; i++) {
        sum += analogReadMilliVolts(pin);
        vTaskDelay(2 / portTICK_PERIOD_MS);
    }
    return sum / sampling_frequency;
}

void SensorBoardAdc::update_value() {
    if (values.empty()) {
        SensorValue item;
        item.name = "电压";
        item.unit = "mV";
        item.value = 0;
        values.push_back(item);
    }

    uint32_t adc_voltage = read_voltage_mv();
    values[0].value = (ADC_R2 + ADC_R1) / static_cast<double>(ADC_R2) * adc_voltage + offset;
    last_update_time = millis();
}
