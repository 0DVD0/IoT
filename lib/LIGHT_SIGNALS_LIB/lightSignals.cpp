#include "lightSignals.h"

void lightSignalsUpdate(
  LightSignals &signals,
  uint16_t rawAdc,
  float lux,
  bool valid,
  uint32_t nowMs, float darkThresholdLux, float lightThresholdLux,
  uint16_t referenceMv
) {
  ++signals.sampleNumber;

  signals.sampledAtMs = nowMs;
  signals.rawAdc = rawAdc;

  // Tensiune estimata pentru alimentare/referinta de 5V.
  signals.voltageMv =
    (uint32_t(rawAdc) * uint32_t(referenceMv)) / 1024UL;

  signals.valid = valid;
  signals.lux = valid ? lux : 0.0f;

  if (!valid) {
    ++signals.invalidCount;
    return;
  }

  // Histerezis: evita schimbarea repetata a starii
  // cand iluminarea oscileaza in apropierea pragului.
  if (lux < darkThresholdLux) {
    signals.dark = true;
  } else if (lux > lightThresholdLux) {
    signals.dark = false;
  }
}

