#include "ESPAsyncWebServer.h"
#include "ArduinoJson.h"

void HandleUdpByte(uint8_t *data, size_t len);

void HandleWsByte(uint8_t *data, size_t len);

void HandleWsText(uint8_t *data, size_t len);

void HandleConnect();
void HandleDisconnect();
