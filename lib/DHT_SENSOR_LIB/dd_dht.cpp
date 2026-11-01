#include "dd_dht.h"
#include <math.h>
DhtTemperatureSensor::DhtTemperatureSensor(uint8_t pin, uint8_t type)
  : device(pin, type), sensorType(type) {}
void DhtTemperatureSensor::begin() { device.begin(); }
bool DhtTemperatureSensor::read(float &temperatureC) {
  const float value = device.readTemperature();
  const float minimum = sensorType == DHT11 ? 0.0f : -40.0f;
  const float maximum = sensorType == DHT11 ? 50.0f : 80.0f;
  if (!isfinite(value) || value < minimum || value > maximum) return false;
  temperatureC = value;
  return true;
}
