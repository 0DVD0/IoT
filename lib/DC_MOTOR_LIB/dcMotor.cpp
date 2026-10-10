#include "dcMotor.h"
namespace {
const int16_t maximumPercent=100;
const int16_t stepPercent=10;
int16_t magnitude(int16_t value) { return value<0 ? -value : value; }
}
uint8_t motorPwm(int16_t percent) {
  return (magnitude(percent)*255L+50)/100;
}
void motorInit(DcMotor &m,uint8_t en,uint8_t a,uint8_t b,uint16_t pause) {
  m={}; m.direction=1; m.reversalPauseMs=pause;
  l298Init(m.driver,en,a,b);
}
void motorStop(DcMotor &m,uint32_t now) {
  if (m.appliedPercent!=0) m.stoppedAtMs=now;
  l298Coast(m.driver);
  m.targetPercent=0; m.appliedPercent=0; m.waiting=false;
}
void motorUpdate(DcMotor &m,uint32_t now) {
  if (!m.targetPercent) return;
  const int8_t wanted=m.targetPercent>0 ? 1 : -1;
  if (m.appliedPercent && (m.appliedPercent>0 ? 1 : -1)!=wanted) {
    l298Coast(m.driver); m.appliedPercent=0; m.stoppedAtMs=now;
  }
  if (m.lastDrivenDirection && wanted!=m.lastDrivenDirection &&
      static_cast<uint32_t>(now-m.stoppedAtMs)<m.reversalPauseMs) {
    m.waiting=true; return;
  }
  m.waiting=false;
  if (m.appliedPercent!=m.targetPercent) {
    l298Drive(m.driver,wanted>0,motorPwm(m.targetPercent));
    m.appliedPercent=m.targetPercent; m.lastDrivenDirection=wanted;
  }
}
bool motorSet(DcMotor &m,int16_t percent,uint32_t now) {
  if (percent < -maximumPercent || percent > maximumPercent) return false;
  if (!percent) { motorStop(m,now); return true; }
  m.direction=percent>0 ? 1 : -1;
  m.targetPercent=percent;
  motorUpdate(m,now); return true;
}
void motorMax(DcMotor &m,uint32_t now) { motorSet(m,m.direction*maximumPercent,now); }
bool motorIncrease(DcMotor &m,uint32_t now) {
  int16_t value=magnitude(m.targetPercent);
  bool limited=value+stepPercent>=maximumPercent;
  value=value+stepPercent>maximumPercent ? maximumPercent : value+stepPercent;
  motorSet(m,m.direction*value,now); return limited;
}
bool motorDecrease(DcMotor &m,uint32_t now) {
  int16_t value=magnitude(m.targetPercent);
  bool limited=value<=stepPercent;
  value=value<stepPercent ? 0 : value-stepPercent;
  motorSet(m,m.direction*value,now); return limited;
}
