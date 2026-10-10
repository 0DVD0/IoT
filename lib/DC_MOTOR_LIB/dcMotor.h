#pragma once
#include "dd_l298.h"
struct DcMotor {
  L298 driver;
  int16_t targetPercent, appliedPercent;
  int8_t direction;
  bool waiting;
  uint32_t stoppedAtMs;
  uint16_t reversalPauseMs;
  int8_t lastDrivenDirection;
};
void motorInit(DcMotor &motor, uint8_t en, uint8_t in1, uint8_t in2,
               uint16_t reversalPauseMs=100);
// Intoarce false pentru o valoare in afara intervalului, fara modificare.
bool motorSet(DcMotor &motor, int16_t percent, uint32_t nowMs);
void motorStop(DcMotor &motor, uint32_t nowMs);
void motorUpdate(DcMotor &motor, uint32_t nowMs);
void motorMax(DcMotor &motor, uint32_t nowMs);
bool motorIncrease(DcMotor &motor, uint32_t nowMs);
bool motorDecrease(DcMotor &motor, uint32_t nowMs);
uint8_t motorPwm(int16_t percent);
