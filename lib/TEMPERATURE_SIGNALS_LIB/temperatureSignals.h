#ifndef TEMPERATURE_SIGNALS_H
#define TEMPERATURE_SIGNALS_H
#include <stdint.h>

struct TemperatureSignals {
  uint32_t sampleNumber;
  uint32_t sampledAtMs;

  uint16_t rawAdc;
  uint16_t voltageMv;

  float temperatureC;
  bool valid;
  bool alert;

  uint32_t errorCount;
};

void temperatureSignalsUpdate(TemperatureSignals &signals, uint16_t rawAdc,
    float temperatureC, bool valid, uint32_t nowMs,
    float lowTemperatureC = 5.0f, float highTemperatureC = 40.0f,
    uint16_t referenceMv = 5000);
#endif
