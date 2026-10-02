#ifndef LDR_CONDITIONING_H
#define LDR_CONDITIONING_H
#include "signalFilters.h"
struct LdrAdcSample {
  uint32_t number, sampledAtMs, droppedCount;
  uint16_t originalAdc, inputAdc;
  bool injected;
};
struct LdrProcessedSignals {
  uint32_t sampleNumber, sampledAtMs, droppedCount, injectedCount, inputRailCount;
  uint16_t originalAdc, inputAdc, medianAdc;
  float filteredAdc, voltage, rawLux, filteredLux, outputLux;
  bool rawValid, filteredValid, saturated, injected, dark;
};
struct LdrConditioner {
  MedianFilter5 median;
  WeightedFilter3 weighted;
  LdrProcessedSignals signals;
};
// Initializare LdrConditioner cu {}. Limitele sunt ale aplicatiei.
void ldrConditionSample(LdrConditioner &state, const LdrAdcSample &sample,
                       float minimumLux = 0.0f, float maximumLux = 1000.0f);
#endif
