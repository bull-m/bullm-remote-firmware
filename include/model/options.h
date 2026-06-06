#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#define OPTIONS_BUILT_IN_IO "builtIn_io"
#define OPTIONS_WALK_EXTENDS "walk_extends"
#define OPTIONS_WALK_SERVOS "walk_servos"
#define OPTIONS_WALK_GROUPS "walk_groups"
#define OPTIONS_WALK_PWMS "walk_pwms"
#define OPTIONS_WALK_DIGITALS "walk_digitals"
#define OPTIONS_CHASSIS "chassis"
#define OPTIONS_PZT "pzt"
#define OPTIONS_SCREEN "screen"
#define OPTIONS_WIFi "wifi"
#define OPTIONS_TOKEN "token"
#define OPTIONS_NETWORK "network"
#define OPTIONS_SENSORS "sensors"


//#define OPTIONS_REPLACE {"\"builtIn\"", "\"name\"", "\"type\"","\"address\"", "\"pin\"", "\"hz\"", "\"def\""}

JsonDocument *OptionsHandle(JsonDocument &data);

void OptionsRemove(const char *key);

void OptionsSet(const char *key, const char *value);

String OptionsGet(const char *key);

bool OptionsGetJson(JsonDocument &json, const char *key);

std::unique_ptr<JsonDocument> OptionsGetJson(const char *key);