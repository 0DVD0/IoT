#ifndef DD_LED_H
#define DD_LED_H

#include <stdint.h>

// LED activ pe HIGH: pin -> rezistor -> anod LED; catod -> GND.
void ledInit(uint8_t pin);
void ledWrite(uint8_t pin, bool on);
void ledOn(uint8_t pin);
void ledOff(uint8_t pin);
void ledToggle(uint8_t pin);
uint8_t ledState(uint8_t pin);

#endif
