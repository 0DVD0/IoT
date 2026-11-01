#pragma once
#include <DHT.h>
#include <stdint.h>

// Driver reutilizabil pentru DHT11 si DHT22. Apelati read la minimum 2 s.
class DhtTemperatureSensor {
 public:
  DhtTemperatureSensor(uint8_t pin, uint8_t type);
  void begin();
  bool read(float &temperatureC);
 private:
  DHT device;
  uint8_t sensorType;
};
