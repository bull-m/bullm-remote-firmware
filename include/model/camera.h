#include <WiFi.h>
#include <AsyncWebSocket.h>
#include <ArduinoJson.h>

#define CAMERA_MODEL_AI_THINKER // Has PSRAM


void CameraStop();

void CameraInit();

JsonDocument * CameraHandle(JsonDocument &data);



