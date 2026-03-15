#include <driver/adc.h>
#include "esp_adc_cal.h"
#include <ArduinoJson.h>
#include <AsyncWebSocket.h>


// ADC_0db: 无衰减，输入电压范围是0 - 1.1V
// ADC_2_5db: 2.5dB衰减，输入电压范围是0 - ~1.5V
// ADC_6db: 6dB衰减，输入电压范围是0 - ~2.2V
// ADC_11db: 11dB衰减，输入电压范围是0 - 3.3V


#define ADC_ATTENUATION ADC_2_5db  // 设置 ADC 衰减，适用于 0 - 3.3V 的输入范围
#define ADC_RESOLUTION 12         // 设置 ADC 分辨率，ESP32-C3 支持最高 12 位
#define V_REF 1.5                 // 参考电压
// 电池电压io
#define PIN_VOLTAGE_IN  9


//#define ADC_CHANNEL     ADC1_CHANNEL_8
#define ADC_ATTEN_DB    ADC_ATTEN_DB_2_5
#define ADC_WIDTH_BIT   ADC_WIDTH_BIT_12
#define ADC_NUM         ADC_UNIT_1      // ADC to characterize (ADC_UNIT_1 or ADC_UNIT_2)
#define ADC_R1          100
#define ADC_R2          10
#define ADC_OFFSET      0 // adb偏移



void AdcInit();

JsonDocument * AdcHandle(JsonDocument &data);
