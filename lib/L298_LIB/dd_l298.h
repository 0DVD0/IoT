#pragma once
#include <stdint.h>
struct L298 { uint8_t enablePin, in1Pin, in2Pin; };
void l298Init(L298 &driver, uint8_t enablePin, uint8_t in1Pin, uint8_t in2Pin);
void l298Coast(L298 &driver);
void l298Drive(L298 &driver, bool forward, uint8_t pwm);
