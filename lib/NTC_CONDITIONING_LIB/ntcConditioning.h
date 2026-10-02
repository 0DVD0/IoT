#ifndef NTC_CONDITIONING_H
#define NTC_CONDITIONING_H
#include "signalFilters.h"
struct NtcAdcSample {
  uint32_t number, sampledAtMs, droppedCount;
  uint16_t originalAdc, inputAdc;
  bool injected;
};
struct NtcProcessedSignals {
  uint32_t sampleNumber, sampledAtMs, droppedCount, injectedCount, inputRailCount;
  uint16_t originalAdc, inputAdc, medianAdc;
  float filteredAdc, voltage, rawTemperatureC, filteredTemperatureC, outputTemperatureC;
  bool rawValid, filteredValid, saturated, injected;
};
struct NtcConditioner {
  MedianFilter5 median;
  WeightedFilter3 weighted;
  NtcProcessedSignals signals;
};
// Initializare NtcConditioner cu {}. Limitele sunt ale aplicatiei.
void ntcConditionSample(NtcConditioner &state, const NtcAdcSample &sample,
                       float minimumC = -20.0f, float maximumC = 80.0f);
#endif
