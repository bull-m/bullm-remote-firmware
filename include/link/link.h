#include <ESPAsyncWebServer.h>
#include "link/handle.h"
#include "entity/NMWiFiUDP.h"
#include "walk/walk.h"
#include "main.h"
#include <WiFi.h>



extern AsyncWebServer server;

extern AsyncWebSocket ws;

extern bool isNetwork;

void HttpInit();
void HttpLoop();

// udp

extern NMWiFiUDP udp;
extern IPAddress udp_serve_ip;
extern int udp_serve_port;
void UdpInit();
void UdpLoop();
void UdpServerAuth();
bool UdpSend(uint8_t type, const uint8_t *data, size_t len);
bool UdpSend(uint8_t type, const uint8_t *data, size_t len, const IPAddress& ip, uint16_t port);


// web
void WebInit();

// ws
void WsInit();
void WsAutoSend(const char *data, size_t len);
void WsAutoSend(JsonDocument &json);
void WsAutoSend(String str);
bool WsIsLink();