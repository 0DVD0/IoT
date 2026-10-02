#include "ldrConditioning.h"
#include "dd_ldr.h"
void ldrConditionSample(LdrConditioner &state, const LdrAdcSample &sample,
                       float minimumLux, float maximumLux) {
  const LdrConfig config;
  LdrProcessedSignals &s = state.signals;
  s.sampleNumber = sample.number;
  s.sampledAtMs = sample.sampledAtMs;
  s.droppedCount = sample.droppedCount;
  s.originalAdc = sample.originalAdc;
  s.inputAdc = sample.inputAdc;
  s.injected = sample.injected;
  if (sample.injected) ++s.injectedCount;
  if (sample.inputAdc == 0 || sample.inputAdc == 1023) ++s.inputRailCount;
  s.medianAdc = medianUpdate(state.median, sample.inputAdc);
  s.filteredAdc = weightedUpdate(state.weighted, s.medianAdc);
  s.voltage = s.filteredAdc * 5.0f / 1024.0f;
  s.rawValid = ldrVoltageToLux(sample.inputAdc * 5.0f / 1024.0f, s.rawLux, config);
  s.filteredValid = ldrVoltageToLux(s.voltage, s.filteredLux, config);
  s.saturated = false;
  s.outputLux = 0;
  if (s.filteredValid) {
    s.outputLux = signalSaturate(s.filteredLux, minimumLux, maximumLux, s.saturated);
    // Histerezis aplicat inainte de saturarea iesirii.
    if (s.filteredLux < 80.0f) s.dark = true;
    else if (s.filteredLux > 120.0f) s.dark = false;
  }
}
