#include "dcMotor.h"
#include "motorCommand.h"
#include <cassert>
#include <stdint.h>
int levels[64]={};
void digitalWrite(uint8_t p,uint8_t v) { levels[p]=v; }
void pinMode(uint8_t,uint8_t) {}
void analogWrite(uint8_t p,int v) { levels[p]=v; }
int main() {
  assert(motorParseCommand(" motor set -100  ").percent==-100);
  assert(motorParseCommand("motor set 101").action==MotorAction::invalid);
  assert(motorParseCommand("motor set 10foo").action==MotorAction::invalid);
  assert(motorParseCommand("motor set 1.5").action==MotorAction::invalid);
  assert(motorParseCommand("motor set ").action==MotorAction::invalid);
  assert(motorParseCommand("motor set 99999999999999999999999999").action==MotorAction::invalid);
  assert(motorParseCommand("motor stop").action==MotorAction::stop);
  DcMotor m; motorInit(m,5,6,7);
  assert(levels[5]==0 && m.direction==1);
  motorSet(m,50,0); assert(levels[5]==128 && levels[6]==1 && levels[7]==0);
  assert(!motorSet(m,101,0) && m.targetPercent==50);
  motorSet(m,-40,10); assert(m.waiting && levels[5]==0);
  motorUpdate(m,109); assert(levels[5]==0);
  motorUpdate(m,110); assert(m.appliedPercent==-40 && levels[5]==102 && levels[7]==1);
  motorIncrease(m,120); assert(m.targetPercent==-50);
  motorDecrease(m,130); assert(m.targetPercent==-40);
  motorSet(m,-95,140); assert(motorIncrease(m,150) && m.targetPercent==-100);
  motorStop(m,160); assert(levels[5]==0 && m.direction==-1);
  motorMax(m,170); assert(m.targetPercent==-100 && m.appliedPercent==-100);
  motorSet(m,100,180); assert(m.waiting);
  motorStop(m,181); motorUpdate(m,1000); assert(levels[5]==0 && m.targetPercent==0);
  motorSet(m,5,1100); assert(motorDecrease(m,1110) && m.appliedPercent==0);
  motorSet(m,20,0xfffffff0UL); motorSet(m,-20,0xfffffff5UL);
  motorUpdate(m,50); assert(m.waiting);
  motorUpdate(m,100); assert(m.appliedPercent==-20);
}
