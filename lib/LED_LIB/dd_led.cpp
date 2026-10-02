#include "dd_led.h"
#include <Arduino.h>

void ledInit(uint8_t pin) {
  digitalWrite(pin, LOW);
  pinMode(pin, OUTPUT);
}

void ledWrite(uint8_t pin, bool on) {
  digitalWrite(pin, on ? HIGH : LOW);
}

void ledOn(uint8_t pin) {
  digitalWrite(pin, HIGH);
}

void ledOff(uint8_t pin) {
  digitalWrite(pin, LOW);
}

void ledToggle(uint8_t pin) {
  digitalWrite(pin, !digitalRead(pin));
}

uint8_t ledState(uint8_t pin) {
  return digitalRead(pin);
}
