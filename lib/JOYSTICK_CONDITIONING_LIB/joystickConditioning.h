#ifndef JOYSTICK_CONDITIONING_H
#define JOYSTICK_CONDITIONING_H
#include "signalFilters.h"
struct JoystickAxisCalibration {
  float minimumVoltage = 0.0f;
  float centerVoltage = 2.5f;
  float maximumVoltage = 5.0f;
  float deadZoneVoltage = 0.20f;
};
struct JoystickConditionConfig {
  float referenceVoltage = 5.0f;
  JoystickAxisCalibration x;
  JoystickAxisCalibration y;
  bool invertX = true;
  bool invertY = false;
};
struct JoystickAdcSample {
  uint32_t number, sampledAtMs, pressCount, droppedCount;
  uint16_t originalX, originalY, inputX, inputY;
  bool buttonPressed, injected;
};
struct JoystickAxisSignals {
  uint16_t originalAdc, inputAdc, medianAdc;
  float filteredAdc, voltage, rawPercent, filteredPercent, outputPercent;
  bool saturated;
};
struct JoystickProcessedSignals {
  uint32_t sampleNumber, sampledAtMs, pressCount, droppedCount, injectedCount;
  JoystickAxisSignals x, y;
  bool buttonPressed, injected;
};
struct JoystickConditioner {
  MedianFilter5 medianX, medianY;
  WeightedFilter3 weightedX, weightedY;
  JoystickProcessedSignals signals;
};
// Calibrare: min < center-deadZone < center+deadZone < max; deadZone >= 0.
float joystickVoltageToPosition(float voltage, const JoystickAxisCalibration &axis, bool invert);
// Starea se initializeaza cu {}. Fiecare axa are propriul istoric.
void joystickConditionSample(JoystickConditioner &state, const JoystickAdcSample &sample,
                             const JoystickConditionConfig &config);
#endif
