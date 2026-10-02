#pragma once
#include "dd_microphone.h"
#include "signalFilters.h"
struct MicrophoneSignals {
  MicrophoneReading reading;
  uint32_t number, dropped;
  uint16_t medianAdc;
  float filteredAdc, rawMillivolts, filteredMillivolts, relativePercent;
  bool saturated;
};
struct MicrophoneProcessor {
  MedianFilter5 median;
  WeightedFilter3 weighted;
};
// fullScaleMv este o referinta relativa, nu o calibrare in dB SPL.
void microphoneProcess(MicrophoneProcessor &state, MicrophoneSignals &signals,
                       bool filterEnabled, float fullScaleMv = 500.0f);
