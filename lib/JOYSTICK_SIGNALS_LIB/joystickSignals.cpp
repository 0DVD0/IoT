#include "joystickSignals.h"

int16_t joystickAxisToPercent(
  uint16_t raw,
  int center,
  int deadZoneAdc,
  bool invert
) {
  if (raw > 1023 || deadZoneAdc < 0 || center <= deadZoneAdc ||
      center >= 1023 - deadZoneAdc) return 0;
  const int delta = (int)raw - center;
  long percent = 0;

  // Scalare separata pe fiecare jumatate a axei.
  // Zona moarta este eliminata din intervalul util.
  if (delta > deadZoneAdc) {
    percent =
      (long)(delta - deadZoneAdc) * 100L
      / (1023 - center - deadZoneAdc);
  } else if (delta < -deadZoneAdc) {
    percent =
      (long)(delta + deadZoneAdc) * 100L
      / (center - deadZoneAdc);
  }

  if (invert) {
    percent = -percent;
  }

  return (int16_t)(percent < -100 ? -100 : (percent > 100 ? 100 : percent));
}

void joystickSignalsUpdate(JoystickSignals &signals, const JoystickReading &reading,
                           const JoystickCalibration &calibration) {
  signals.rawX = reading.rawX;
  signals.rawY = reading.rawY;
  signals.sampledAtMs = reading.sampledAtMs;
  signals.buttonPressed = reading.buttonPressed;
  if (reading.pressEvent) ++signals.pressCount;
  ++signals.sampleNumber;
  signals.xPercent = joystickAxisToPercent(reading.rawX, calibration.xCenter,
      calibration.deadZoneAdc, calibration.invertX);
  signals.yPercent = joystickAxisToPercent(reading.rawY, calibration.yCenter,
      calibration.deadZoneAdc, calibration.invertY);
}
