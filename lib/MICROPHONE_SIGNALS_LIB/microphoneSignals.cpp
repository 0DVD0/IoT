#include "microphoneSignals.h"
void microphoneProcess(MicrophoneProcessor &state, MicrophoneSignals &s,
                       bool filterEnabled, float fullScaleMv) {
  s.medianAdc = filterEnabled ? medianUpdate(state.median, s.reading.peakToPeakAdc)
                             : s.reading.peakToPeakAdc;
  s.filteredAdc = filterEnabled ? weightedUpdate(state.weighted, s.medianAdc)
                               : s.medianAdc;
  s.rawMillivolts = s.reading.peakToPeakAdc * 5000.0f / 1023.0f;
  s.filteredMillivolts = s.filteredAdc * 5000.0f / 1023.0f;
  const float percent = fullScaleMv > 0 ? s.filteredMillivolts * 100.0f / fullScaleMv : 0;
  s.relativePercent = signalSaturate(percent, 0, 100, s.saturated);
}
