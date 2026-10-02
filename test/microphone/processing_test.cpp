// Host: c++ -std=c++11 -Ilib/MICROPHONE_LIB -Ilib/MICROPHONE_SIGNALS_LIB
// -Ilib/SIGNAL_FILTER_LIB test/microphone/processing_test.cpp
// lib/MICROPHONE_SIGNALS_LIB/microphoneSignals.cpp
// lib/SIGNAL_FILTER_LIB/signalFilters.cpp -o /tmp/microphone-test
#include "microphoneSignals.h"
#include <cassert>
#include <cmath>
int main() {
  MicrophoneProcessor p = {};
  MicrophoneSignals s = {};
  s.reading.peakToPeakAdc = 10;
  microphoneProcess(p, s, false);
  assert(std::fabs(s.rawMillivolts - 48.8759f) < 0.01f);
  assert(s.filteredAdc == 10 && !s.saturated);
  microphoneProcess(p, s, true);
  s.reading.peakToPeakAdc = 1023;
  microphoneProcess(p, s, true);
  assert(s.medianAdc == 10 && s.filteredAdc == 10);
  for (int i=0; i<8; ++i) microphoneProcess(p, s, true);
  assert(s.filteredAdc == 1023 && s.saturated && s.relativePercent == 100);
  s.reading.peakToPeakAdc = 0;
  for (int i=0; i<8; ++i) microphoneProcess(p, s, true);
  assert(s.relativePercent == 0 && !s.saturated);
}
