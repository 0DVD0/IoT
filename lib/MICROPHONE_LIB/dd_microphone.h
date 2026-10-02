#pragma once
#include <stdint.h>
struct MicrophoneReading {
  uint16_t minimumAdc, maximumAdc, peakToPeakAdc, sampleCount;
  uint32_t sampledAtMs;
  bool clipped;
};
void microphoneInit(uint8_t pin);
// Echantillonnage rapid intr-o fereastra; intreruperile raman active.
MicrophoneReading microphoneReadWindow(uint8_t pin, uint16_t windowMs = 20);
