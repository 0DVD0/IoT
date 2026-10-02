#include "dd_button.h"
#include <Arduino.h>

void buttonInit(Button &button, uint8_t pin, uint16_t debounceMs, bool activeLow) {
  pinMode(pin, activeLow ? INPUT_PULLUP : INPUT);
  button = {pin, activeLow, digitalRead(pin) == (activeLow ? LOW : HIGH),
            false, false, millis(), debounceMs};
}
void buttonUpdate(Button &button, uint32_t nowMs) {
  const bool pressed = digitalRead(button.pin) == (button.activeLow ? LOW : HIGH);
  if (pressed != button.rawPressed) {
    button.rawPressed = pressed;
    button.changedAtMs = nowMs;
  }
  if (pressed != button.stablePressed &&
      static_cast<uint32_t>(nowMs - button.changedAtMs) >= button.debounceMs) {
    button.stablePressed = pressed;
    if (pressed) button.pressPending = true;
  }
}
bool buttonIsPressed(const Button &button) { return button.stablePressed; }
bool buttonWasPressed(Button &button) {
  const bool pending = button.pressPending;
  button.pressPending = false;
  return pending;
}
