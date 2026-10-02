#ifndef DD_BUTTON_H
#define DD_BUTTON_H
#include <stdint.h>

// Un obiect per buton. activeLow=true: INPUT_PULLUP, buton la GND.
// activeLow=false: INPUT, rezistenta externa pull-down, buton la 5V.
struct Button {
  uint8_t pin;
  bool activeLow;
  bool rawPressed;
  bool stablePressed;
  bool pressPending;
  uint32_t changedAtMs;
  uint16_t debounceMs;
};
void buttonInit(Button &button, uint8_t pin, uint16_t debounceMs = 30,
                bool activeLow = true);
void buttonUpdate(Button &button, uint32_t nowMs);
bool buttonIsPressed(const Button &button);
// Returneaza evenimentul o singura data; mentinerea apasata nu repeta.
bool buttonWasPressed(Button &button);
#endif
