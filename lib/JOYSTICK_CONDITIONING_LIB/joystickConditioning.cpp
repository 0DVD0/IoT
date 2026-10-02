#include "joystickConditioning.h"
float joystickVoltageToPosition(float voltage, const JoystickAxisCalibration &axis, bool invert) {
  const float lower = axis.centerVoltage - axis.deadZoneVoltage;
  const float upper = axis.centerVoltage + axis.deadZoneVoltage;
  float position = 0;
  if (voltage > upper) position = (voltage - upper) * 100 / (axis.maximumVoltage - upper);
  else if (voltage < lower) position = (voltage - lower) * 100 / (lower - axis.minimumVoltage);
  return invert ? -position : position;
}
namespace {
void conditionAxis(JoystickAxisSignals &s, uint16_t original, uint16_t input,
                   const JoystickAxisCalibration &axis, float reference, bool invert,
                   MedianFilter5 &median, WeightedFilter3 &weighted) {
  s.originalAdc = original;
  s.inputAdc = input;
  s.medianAdc = medianUpdate(median, input);
  s.filteredAdc = weightedUpdate(weighted, s.medianAdc);
  s.voltage = s.filteredAdc * reference / 1023.0f;
  s.rawPercent = joystickVoltageToPosition(input * reference / 1023.0f, axis, invert);
  s.filteredPercent = joystickVoltageToPosition(s.voltage, axis, invert);
  s.outputPercent = signalSaturate(s.filteredPercent, -100, 100, s.saturated);
}
}
void joystickConditionSample(JoystickConditioner &state, const JoystickAdcSample &sample,
                             const JoystickConditionConfig &config) {
  JoystickProcessedSignals &s = state.signals;
  s.sampleNumber = sample.number;
  s.sampledAtMs = sample.sampledAtMs;
  s.pressCount = sample.pressCount;
  s.droppedCount = sample.droppedCount;
  s.buttonPressed = sample.buttonPressed;
  s.injected = sample.injected;
  if (sample.injected) ++s.injectedCount;
  conditionAxis(s.x, sample.originalX, sample.inputX, config.x, config.referenceVoltage,
                config.invertX, state.medianX, state.weightedX);
  conditionAxis(s.y, sample.originalY, sample.inputY, config.y, config.referenceVoltage,
                config.invertY, state.medianY, state.weightedY);
}
