#pragma once
#include <stdint.h>
// One sensor per MCU; ECHO must support an external interrupt.
bool hcsr04Init(uint8_t triggerPin, uint8_t echoPin);
void hcsr04Start();
bool hcsr04TakePulse(uint32_t &durationUs);
void hcsr04Cancel();
