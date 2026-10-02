#pragma once
#include <stdint.h>
struct EncoderReading { int32_t position; uint32_t invalidTransitions; };
// Driver pentru un encoder pe doua intrari cu intreruperi externe.
bool encoderInit(uint8_t clkPin, uint8_t dtPin);
EncoderReading encoderRead();
