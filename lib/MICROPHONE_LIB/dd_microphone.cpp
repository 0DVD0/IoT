#include "dd_microphone.h"
#include <Arduino.h>
void microphoneInit(uint8_t pin) {
  pinMode(pin, INPUT);
  analogReference(DEFAULT);
}
MicrophoneReading microphoneReadWindow(uint8_t pin, uint16_t windowMs) {
  MicrophoneReading r = {};
  r.minimumAdc = 1023;
  analogRead(pin);
  const uint32_t start = micros();
  do {
    const uint16_t value = analogRead(pin);
    if (value < r.minimumAdc) r.minimumAdc = value;
    if (value > r.maximumAdc) r.maximumAdc = value;
    if (r.sampleCount < 65535) ++r.sampleCount;
  } while (static_cast<uint32_t>(micros() - start) < uint32_t(windowMs) * 1000UL);
  r.peakToPeakAdc = r.maximumAdc - r.minimumAdc;
  r.clipped = r.minimumAdc == 0 || r.maximumAdc == 1023;
  r.sampledAtMs = millis();
  return r;
}
