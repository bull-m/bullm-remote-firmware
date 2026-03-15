#include "model/camera.h"
#include "esp_camera.h"
#include <Preferences.h>
#include <esp_task_wdt.h>
#include <main.h>
#include "link/link.h"
#include "model/error.h"


bool camera_pause = false;
bool runing = false;
bool loaded = false;

void CameraStop() {
    runing = false;
}

int start() {
    if (loaded) return -1;
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.frame_size = FRAMESIZE_SVGA;
    config.pixel_format = PIXFORMAT_JPEG;
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    config.fb_location = CAMERA_FB_IN_DRAM;
    config.jpeg_quality = 10;
    if (psramFound()) {
        Serial.println("psramFound");
        config.fb_count = 3;
        config.fb_location = CAMERA_FB_IN_PSRAM;
        config.grab_mode = CAMERA_GRAB_LATEST;
        // config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    } else {
        Serial.println("no-psramFound");
        config.fb_count = 1;
        config.fb_location = CAMERA_FB_IN_DRAM;
    }
    esp_task_wdt_reset(); // 喂食狗
    // 初始化相机
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x", err);
        Serial.println("");
        ErrorAdd("相机启动失败: "+ String(esp_err_to_name(err)) +" code: 0x" + String(err, 16));
        // 关闭摄像头电源
        pinMode(PWDN_GPIO_NUM, OUTPUT);
        digitalWrite(PWDN_GPIO_NUM, HIGH);
        runing = false; // 取消运行
        loaded = false;
        return err;
    }
    esp_camera_load_from_nvs("camera_settings");
    Serial.println("Camera init");
    vTaskDelay(pdMS_TO_TICKS(30));
    loaded = true;
    return -1;
}

void stop() {
    if (!loaded) return;
    Serial.println("Camera deinit");
    // esp_camera_return_all();
    if (esp_camera_deinit() == ESP_OK) {
        Serial.println("Camera deinit success");
    } else {
        Serial.println("Camera deinit error");
    }
    // 关闭摄像头电源
    pinMode(PWDN_GPIO_NUM, OUTPUT);
    digitalWrite(PWDN_GPIO_NUM, HIGH);
    loaded = false;
    runing = false;
}

bool send(camera_fb_t *fb) {
    if (!runing || udp_serve_port == 0 || !fb) return false;
    return UdpSend(0x01, fb->buf, fb->len);
}


JsonDocument *handleTypeGetVideoStatus() {
    auto *req = new JsonDocument();
    auto &json = *req;
    sensor_t *s = esp_camera_sensor_get();
    if(s == nullptr){
        ErrorAdd("摄像头无法连接，请检查");
        json["info_name"] = "摄像头未连接";
        return req;
    }
    camera_sensor_info_t *info = esp_camera_sensor_get_info(&s->id);
    json["xclk"] = s->xclk_freq_hz;
//    json["pixformat"] = s->pixformat;
    json["framesize"] = s->status.framesize;
    json["quality"] = s->status.quality;
    json["brightness"] = s->status.brightness;
    json["contrast"] = s->status.contrast;
    json["saturation"] = s->status.saturation;
    json["sharpness"] = s->status.sharpness;
    json["special_effect"] = s->status.special_effect;
    json["wb_mode"] = s->status.wb_mode;
    json["awb"] = s->status.awb;
    json["awb_gain"] = s->status.awb_gain;
    json["aec"] = s->status.aec;
    json["aec2"] = s->status.aec2;
    json["ae_level"] = s->status.ae_level;
    json["aec_value"] = s->status.aec_value;
    json["agc"] = s->status.agc;
    json["agc_gain"] = s->status.agc_gain;
    json["gainceiling"] = s->status.gainceiling;
    json["bpc"] = s->status.bpc;
    json["wpc"] = s->status.wpc;
    json["raw_gma"] = s->status.raw_gma;
    json["lenc"] = s->status.lenc;
    json["hmirror"] = s->status.hmirror;
    json["dcw"] = s->status.dcw;
    json["colorbar"] = s->status.colorbar;
    json["vflip"] = s->status.vflip;

    json["info_name"] = info->name;
    json["info_max_size"] = info->max_size;
    json["info_model"] = info->model;
    json["info_pid"] = info->pid;
    json["info_support_jpeg"] = info->support_jpeg;
    return req;
}

int handleTypeSetVideoStatus(JsonDocument &doc) {
    sensor_t *s = esp_camera_sensor_get();
    String name = doc["name"];
    int val = atoi(doc["value"]);
    int res = 0;
    if (name == "reset") {
        if (s->reset(s) == ESP_OK) {
            s->set_framesize(s, FRAMESIZE_SVGA);
            s->set_xclk(s, LEDC_TIMER_0, 20);
            s->set_quality(s, 10);
            s->set_contrast(s, 0);
            s->set_brightness(s, 0);
            s->set_saturation(s, 0);
            s->set_special_effect(s, 0);
        }
    } else if (name == "framesize") {
        if (s->pixformat == PIXFORMAT_JPEG) {
            res = s->set_framesize(s, (framesize_t) val);
        }
    } else if (name == "xclk")
        res = s->set_xclk(s, LEDC_TIMER_0, val);
    else if (name == "quality")
        res = s->set_quality(s, val);
    else if (name == "contrast") // 对比度
        res = s->set_contrast(s, val);
    else if (name == "brightness") // 亮度
        res = s->set_brightness(s, val);
    else if (name == "saturation") // 饱和度
        res = s->set_saturation(s, val);
    else if (name == "gainceiling")
        res = s->set_gainceiling(s, (gainceiling_t) val);
    else if (name == "colorbar")
        res = s->set_colorbar(s, val);
    else if (name == "awb")
        res = s->set_whitebal(s, val);
    else if (name == "agc")
        res = s->set_gain_ctrl(s, val);
    else if (name == "aec")
        res = s->set_exposure_ctrl(s, val);
    else if (name == "hmirror")
        res = s->set_hmirror(s, val);
    else if (name == "vflip")
        res = s->set_vflip(s, val);
    else if (name == "awb_gain")
        res = s->set_awb_gain(s, val);
    else if (name == "agc_gain")
        res = s->set_agc_gain(s, val);
    else if (name == "aec_value")
        res = s->set_aec_value(s, val);
    else if (name == "aec2")
        res = s->set_aec2(s, val);
    else if (name == "dcw")
        res = s->set_dcw(s, val);
    else if (name == "bpc")
        res = s->set_bpc(s, val);
    else if (name == "wpc")
        res = s->set_wpc(s, val);
    else if (name == "raw_gma")
        res = s->set_raw_gma(s, val);
    else if (name == "lenc")
        res = s->set_lenc(s, val);
    else if (name == "special_effect")
        res = s->set_special_effect(s, val);
    else if (name == "wb_mode")
        res = s->set_wb_mode(s, val);
    else if (name == "ae_level")
        res = s->set_ae_level(s, val);
    else {
        res = -1;
    }
    esp_camera_save_to_nvs("camera_settings");
    esp_camera_return_all();
    return res;
}


// 视频传输性能监控
volatile unsigned long frame_captured_count = 0;  // 采集帧计数
volatile unsigned long frame_sent_count = 0;      // 发送帧计数
volatile unsigned long frame_dropped_count = 0;   // 丢弃帧计数
volatile unsigned long ws_send_fail_count = 0;    // WebSocket发送失败计数

volatile bool snapshot_in_progress = false; // 抓拍期间暂停实时采集

volatile int g_target_fps = 0; // 新增：0=不限，>0 则按该FPS限速发送

typedef camera_fb_t *fb_ptr_t;
QueueHandle_t qFrames;

[[noreturn]] void taskCamCapture(void *) {
    while (true) {
        // 暂停
        if (snapshot_in_progress) {
            vTaskDelay(pdMS_TO_TICKS(5));
            continue;
        }
        // 还没有初始化
        if (runing && !loaded) {
            start();
        }
        if (!runing && loaded) {
            stop();
        }
        // 没有启动或者没有初始化
        if (!runing || !loaded) {
            vTaskDelay(pdMS_TO_TICKS(20));
            continue;
        }

        camera_fb_t *fb = esp_camera_fb_get();
        if (!fb) {
            vTaskDelay(pdMS_TO_TICKS(2));
            continue;
        }
        frame_captured_count++;
        // 帧格式不对
        if (fb->format != PIXFORMAT_JPEG) {
            esp_camera_fb_return(fb);
            continue;
        }
        // 添加到队列
        if (xQueueSend(qFrames, &fb, 0) == pdPASS) {
            continue;
        }
        // 队列满了，丢弃最旧的帧
        fb_ptr_t drop = nullptr;
        if (xQueueReceive(qFrames, &drop, 0) == pdPASS) {
            if (drop) {
                Serial.printf("[CAM] WARNING: frame drop %lu\n", frame_dropped_count);
                esp_camera_fb_return(drop);
                frame_dropped_count++;  // 丢帧
            }
        }
        xQueueSend(qFrames, &fb, 0);
    }
}

[[noreturn]] void taskCamSend(void *) {
    static TickType_t lastTick = 0;
    unsigned long last_sent_time = 0; // 上次发送时间

    while (true) {
        fb_ptr_t fb = nullptr;
        if (xQueueReceive(qFrames, &fb, pdMS_TO_TICKS(100)) != pdPASS) {
            // 长时间没有帧
            unsigned long now = millis();
            if (runing && loaded && last_sent_time > 0 && (now - last_sent_time) > 3000) {
                Serial.printf("[CAM] WARNING: No frame sent for %lu ms\n", now - last_sent_time);
            }
        }
        if (!fb) continue;
        // 没有启动或者没有初始化
        if (!runing || !loaded) {
            esp_camera_fb_return(fb);
            continue;
        }

        // 发送节流：若设置了目标FPS，则按周期发，丢弃多余帧由 qFrames 机制承担
        if (g_target_fps > 0) {
            const int period_ms = 1000 / g_target_fps;
            TickType_t now = xTaskGetTickCount();
            int elapsed = (now - lastTick) * portTICK_PERIOD_MS;
            if (elapsed < period_ms) vTaskDelay(pdMS_TO_TICKS(period_ms - elapsed));
            lastTick = xTaskGetTickCount();
        }

        unsigned long send_start = millis();

//        uint8_t* buf_with_header = new uint8_t[fb->len + 1];
//        buf_with_header[0] = 0x01;
//        memcpy(buf_with_header + 1, fb->buf, fb->len);
//        bool ok = ws.binaryAll((const char *) buf_with_header, fb->len + 1);
//        delete[] buf_with_header;

        bool ok = send(fb);
//        bool ok = ws.binaryAll((const char *) fb->buf, fb->len);
        esp_camera_fb_return(fb);
        // bool ok = ws.sendBinary((const char*)fb->buf, fb->len);
        unsigned long send_time = millis() - send_start;
        if (ok) {
            frame_sent_count++;
            last_sent_time = millis();
            // 监控发送耗时
            if (send_time > 100) {
                Serial.printf("[CAM] WARNING: send took %lu ms (size=%u)\n", send_time, fb->len);
            }
        } else {
            ws_send_fail_count++;
            Serial.println("[CAM] ERROR: WebSocket send failed, closing...");
            // 尝试重新打开WebSocket
            //   wsCam.close();
            //   cam_ws_ready = false;
        }
    }
}

void CameraInit() {
    qFrames = xQueueCreate(3, sizeof(fb_ptr_t));  // 3个缓冲，减少丢帧
    xTaskCreatePinnedToCore(taskCamCapture, "TaskCamCapture", 10240, nullptr, 4, nullptr, 1);
    xTaskCreatePinnedToCore(taskCamSend, "TaskCamSend", 8192, nullptr, 3, nullptr, 1);
}

JsonDocument *CameraHandle(JsonDocument &data) {
    String mode = data["mode"];
    if (mode == "start") {
        if (!isNetwork && !data["port"].isNull()) {
            udp_serve_port = data["port"].as<int>();
        }
        camera_pause = false;
        runing = true; // 启动摄像头
    } else if (mode == "stop") {
        CameraStop();
    } else if (mode == "pause") {
        camera_pause = true;
    } else if (mode == "get") {
        return handleTypeGetVideoStatus();
    } else if (mode == "set") {
        handleTypeSetVideoStatus(data);
    }
    return new JsonDocument();
}
