#ifndef DD_JOYSTICK_H
#define DD_JOYSTICK_H
#include <stdint.h>
#include "dd_button.h"
struct Joystick {
  uint8_t xPin;
  uint8_t yPin;
  Button button;
};
struct JoystickReading {
  uint16_t rawX;
  uint16_t rawY;
  uint32_t sampledAtMs;
  bool buttonPressed;
  bool pressEvent;
};
// Init o singura data per dispozitiv; buton SEL activ LOW, INPUT_PULLUP.
void joystickInit(Joystick &joystick, uint8_t xPin, uint8_t yPin,
                  uint8_t buttonPin, uint16_t debounceMs = 30);
JoystickReading joystickRead(Joystick &joystick);
#endif
