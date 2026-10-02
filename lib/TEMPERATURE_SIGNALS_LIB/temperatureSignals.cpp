#include "temperatureSignals.h"

void temperatureSignalsUpdate(
  TemperatureSignals &signals,
  uint16_t rawAdc,
  float temperatureC,
  bool valid,
  uint32_t nowMs, float lowTemperatureC, float highTemperatureC,
  uint16_t referenceMv
) {
  ++signals.sampleNumber;

  signals.sampledAtMs = nowMs;
  signals.rawAdc = rawAdc;

  // Estimare tensiune, presupunand referinta de 5000 mV.
  signals.voltageMv =
    (uint32_t(rawAdc) * uint32_t(referenceMv) + 511UL) / 1023UL;

  signals.valid = valid;
  signals.temperatureC = valid ? temperatureC : 0.0f;

  signals.alert =
    valid &&
    (temperatureC < lowTemperatureC ||
     temperatureC > highTemperatureC);

  if (!valid) {
    ++signals.errorCount;
  }
}

