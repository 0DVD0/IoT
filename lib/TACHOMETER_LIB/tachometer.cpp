#include "tachometer.h"
#include <Arduino.h>
#include <util/atomic.h>
namespace {
volatile uint32_t count=0,lastEdge=0;
uint32_t previousCount=0,previousTime=0;
void edge() {++count;lastEdge=millis();}
}
bool tachometerInit(uint8_t pin) {
  if(digitalPinToInterrupt(pin)==NOT_AN_INTERRUPT) return false;
  pinMode(pin,INPUT_PULLUP);
  tachometerReset();
  attachInterrupt(digitalPinToInterrupt(pin),edge,RISING);
  return true;
}
void tachometerReset() {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {count=0;lastEdge=millis();previousTime=lastEdge;}
  previousCount=0;
}
TachometerSample tachometerRead() {
  uint32_t current,edgeTime,now;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {current=count;edgeTime=lastEdge;now=millis();}
  TachometerSample result={current-previousCount,now-previousTime,now-edgeTime};
  previousCount=current;previousTime=now;return result;
}
float tachometerRpm(const TachometerSample &s,uint16_t ppr) {
  return ppr && s.elapsedMs ? s.pulses*60000.0f/(ppr*(float)s.elapsedMs) : 0;
}
