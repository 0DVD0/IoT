#include "hcsr04.h"
#include <Arduino.h>
#include <util/atomic.h>
namespace {
uint8_t trigger,echo;
volatile bool armed=false, risingSeen=false, ready=false;
volatile uint32_t risingUs=0, pulseUs=0;
void onEcho() {
  if(!armed) return;
  const uint32_t now=micros();
  if(digitalRead(echo)==HIGH) {risingUs=now;risingSeen=true;}
  else if(risingSeen) {pulseUs=now-risingUs;ready=true;armed=false;}
}
}
bool hcsr04Init(uint8_t t,uint8_t e) {
  if(digitalPinToInterrupt(e)==NOT_AN_INTERRUPT) return false;
  trigger=t;echo=e;digitalWrite(trigger,LOW);pinMode(trigger,OUTPUT);pinMode(echo,INPUT);
  hcsr04Cancel();attachInterrupt(digitalPinToInterrupt(echo),onEcho,CHANGE);return true;
}
void hcsr04Start() {
  digitalWrite(trigger,LOW);delayMicroseconds(2);
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {armed=true;risingSeen=false;ready=false;}
  digitalWrite(trigger,HIGH);delayMicroseconds(10);digitalWrite(trigger,LOW);
}
bool hcsr04TakePulse(uint32_t &duration) {
  bool result=false;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    if(ready) {duration=pulseUs;ready=false;result=true;}
  }
  return result;
}
void hcsr04Cancel() {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {armed=false;ready=false;risingSeen=false;}
}
