#pragma once
#include <stdint.h>
struct TachometerSample { uint32_t pulses; uint32_t elapsedMs; uint32_t quietMs; };
bool tachometerInit(uint8_t pin);
void tachometerReset();
TachometerSample tachometerRead();
float tachometerRpm(const TachometerSample &sample, uint16_t pulsesPerRevolution);
