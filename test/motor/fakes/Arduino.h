#pragma once
#include <stdint.h>
#define HIGH 1
#define LOW 0
#define OUTPUT 1
void digitalWrite(uint8_t pin, uint8_t level);
void pinMode(uint8_t pin, uint8_t mode);
void analogWrite(uint8_t pin, int value);
