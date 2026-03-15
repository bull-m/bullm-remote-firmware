#include "model/adc.h"
#include <driver/adc.h>
#include "esp_adc_cal.h"
#include <esp32-hal.h>
#include <AsyncWebSocket.h>
#include "link/link.h"

esp_adc_cal_characteristics_t *adcChar;

uint32_t adc_read_val();

uint32_t uID;
uint32_t msg_i;


JsonDocument *AdcHandle(JsonDocument &data) {
    String mode = data["mode"];
    if (mode == "get") {
        uID = 1;
        msg_i = data["i"];
    }
    return nullptr;
}


uint32_t adc_read_val() {
    int samplingFrequency = 500; // 采样频率（可调）
    long sum = 0;                // 采样和
    double samples = 0.0;         // 采样平均值
    for (int i = 0; i < samplingFrequency; i++) {
#ifdef ADC_CHANNEL
        sum += adc1_get_raw(ADC_CHANNEL); // Take an ADC1 reading from a single channel.
#endif
        // delayMicroseconds(1000);
        vTaskDelay(5 / portTICK_PERIOD_MS); // 延时1秒
    }
    samples = sum / (double) samplingFrequency;
    // 2.调用 API 函数将 ADC 值转换为电压（在调用此函数之前，必须初始化特征结构，也就是步骤 1）
    uint32_t voltage = esp_adc_cal_raw_to_voltage(samples, adcChar);
    return voltage; // 单位(mV)
}


// 启动adc的任务
void AdcInit() {
#ifdef ADC_CHANNEL
    adc1_config_width(
            ADC_WIDTH_BIT);                     // Configure ADC1 capture width, meanwhile enable output invert for ADC1. The configuration is for all channels of ADC1.
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB); // Configure the ADC2 channel, including setting attenuation.
    /*
     ! 对于 ESP32s2，仅支持两点值校准，仅支持ADC_WIDTH_BIT_13。参数default_vref未使用。
     * 使用 ADC 校准转换测量电压需要两步：
     * 1.获取 ADC-Voltage 特征曲线
     * 2.调用 API 函数将 ADC 值转换位电压
     * 对于带有eFuse ADC校准位的电路板，可以使用esp_adc_cal_raw_to_voltage（）来获得校准的转换结果。
     * 这些结果代表实际电压（以mV为单位）。无需通过公式 （Vout = Dout * Vmax / Dmax） 转换这些数据。
     * 如果在没有电子保险丝 ADC 校准位的电路板上使用 ADC 校准 API，则会生成警告。
     * esp_err_t esp_adc_cal_check_efuse（esp_adc_cal_value_t value_type)
     * 检查 ADC 校准值是否已刻录到电子保险丝中。
     */
    adcChar = (esp_adc_cal_characteristics_t *) calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t cal_mode = esp_adc_cal_characterize(ADC_NUM, ADC_ATTEN_DB, ADC_WIDTH_BIT,
                                                            ESP_ADC_CAL_VAL_DEFAULT_VREF,
                                                            adcChar); // 1.获取 ADC-Voltage 特征曲线
    xTaskCreatePinnedToCore([](void *pvParameters) {
        while (true) {
            if (uID != -1) {
                // /*通过R1&R2推算电池电压*/
                double battery_voltage = (ADC_R2 + ADC_R1) / ((double) ADC_R2) * adc_read_val() + ADC_OFFSET;
                /*adc端电压*/
//                double adc_voltage = (double) adc_read_val() / 1000.0f;
                if (uID != -1) {
                    WsAutoSend("{\"i\":" + String(msg_i) + ", \"voltage\":" + battery_voltage + "}");
                    uID = -1;
                }
            }
            vTaskDelay(50 / portTICK_PERIOD_MS); // 延时
        }
    }, "TaskAdc", 2048, NULL, 1, NULL, 0);
#endif
}
