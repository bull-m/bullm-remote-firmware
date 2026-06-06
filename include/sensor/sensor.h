#include <map>
#include "model/options.h"
#include "sensor/basic/BasicSensor.h"

extern std::map<String, BasicSensor *> sensors;

void SensorInit();
JsonDocument *SensorHandle(JsonDocument &data);