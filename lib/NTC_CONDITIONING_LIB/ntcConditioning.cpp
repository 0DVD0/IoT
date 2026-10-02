#include "ntcConditioning.h"
#include "dd_ntc.h"
void ntcConditionSample(NtcConditioner &state, const NtcAdcSample &sample,
                       float minimumC, float maximumC) {
  NtcProcessedSignals &s = state.signals;
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
  s.voltage = s.filteredAdc * 5.0f / 1023.0f;
  s.rawValid = ntcVoltageToTemperature(sample.inputAdc * 5.0f / 1023.0f, s.rawTemperatureC);
  s.filteredValid = ntcVoltageToTemperature(s.voltage, s.filteredTemperatureC);
  s.saturated = false;
  s.outputTemperatureC = 0;
  if (s.filteredValid)
    s.outputTemperatureC = signalSaturate(s.filteredTemperatureC, minimumC, maximumC, s.saturated);
}
