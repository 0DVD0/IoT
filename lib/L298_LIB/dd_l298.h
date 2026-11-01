#pragma once
#include <stdint.h>
// Aceeasi interfata EN + IN1 + IN2 este compatibila cu o punte L293D.
// Numele API ramane pentru compatibilitatea cu modulele existente.
// L293D: enablePin -> 1,2EN; in1Pin -> 1A; in2Pin -> 2A.
struct L298 { uint8_t enablePin, in1Pin, in2Pin; };
void l298Init(L298 &driver, uint8_t enablePin, uint8_t in1Pin, uint8_t in2Pin);
void l298Coast(L298 &driver);
void l298Drive(L298 &driver, bool forward, uint8_t pwm);
