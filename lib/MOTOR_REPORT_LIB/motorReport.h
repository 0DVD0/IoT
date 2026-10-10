#pragma once
#include <stdint.h>
struct MotorSnapshot {
  int16_t target, applied;
  int8_t direction;
  uint8_t pwm;
  bool waiting;
  uint32_t commandNumber;
  const char *message;
};
void motorPrintReport(const MotorSnapshot &s);
void motorPrintHelp();
