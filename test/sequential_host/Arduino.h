#pragma once
#include <stdint.h>
#define LOW 0
#define HIGH 1
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2
extern uint32_t testTime;
extern int testPins[70];
inline uint32_t millis() { return testTime; }
inline void delay(unsigned long ms) { testTime += ms; }
inline void pinMode(uint8_t pin, int mode) { if (mode == INPUT_PULLUP) testPins[pin] = HIGH; }
inline int digitalRead(uint8_t pin) { return testPins[pin]; }
inline void digitalWrite(uint8_t pin, int value) { testPins[pin] = value; }
