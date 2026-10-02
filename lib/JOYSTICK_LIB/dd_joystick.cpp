#include "dd_joystick.h"
#include <Arduino.h>
void joystickInit(Joystick &joystick, uint8_t xPin, uint8_t yPin,
                  uint8_t buttonPin, uint16_t debounceMs) {
  joystick.xPin = xPin;
  joystick.yPin = yPin;
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  analogReference(DEFAULT);
  buttonInit(joystick.button, buttonPin, debounceMs, true);
}
JoystickReading joystickRead(Joystick &joystick) {
  JoystickReading reading = {};
  // Eliminam prima conversie dupa schimbarea canalului ADC.
  analogRead(joystick.xPin);
  reading.rawX = analogRead(joystick.xPin);
  analogRead(joystick.yPin);
  reading.rawY = analogRead(joystick.yPin);
  reading.sampledAtMs = millis();
  buttonUpdate(joystick.button, reading.sampledAtMs);
  reading.buttonPressed = buttonIsPressed(joystick.button);
  reading.pressEvent = buttonWasPressed(joystick.button);
  return reading;
}
