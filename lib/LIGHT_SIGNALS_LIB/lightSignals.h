#ifndef LIGHT_SIGNALS_H
#define LIGHT_SIGNALS_H
#include <stdint.h>

struct LightSignals {
  uint32_t sampleNumber;
  uint32_t sampledAtMs;
  uint32_t invalidCount;

  uint16_t rawAdc;
  uint16_t voltageMv;

  float lux;
  bool valid;
  bool dark;
};

void lightSignalsUpdate(LightSignals &signals, uint16_t rawAdc, float lux,
    bool valid, uint32_t nowMs, float darkThresholdLux = 80.0f,
    float lightThresholdLux = 120.0f, uint16_t referenceMv = 5000);
#endif
