#ifndef JOYSTICK_SIGNALS_H
#define JOYSTICK_SIGNALS_H
#include <stdint.h>
#include "dd_joystick.h"

struct JoystickSignals {
  uint32_t sampleNumber;
  uint32_t sampledAtMs;
  uint32_t pressCount;

  uint16_t rawX;
  uint16_t rawY;

  int16_t xPercent;
  int16_t yPercent;

  bool buttonPressed;
};

struct JoystickCalibration {
  int16_t xCenter = 512;
  int16_t yCenter = 512;
  int16_t deadZoneAdc = 40;
  bool invertX = true;
  bool invertY = false;
};
// Pozitie electrica relativa (%), nu unghi fizic.
int16_t joystickAxisToPercent(uint16_t raw, int center, int deadZoneAdc, bool invert);
void joystickSignalsUpdate(JoystickSignals &signals, const JoystickReading &reading,
                           const JoystickCalibration &calibration);
#endif
