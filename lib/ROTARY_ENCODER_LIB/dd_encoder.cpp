#include "dd_encoder.h"
#include "quadrature.h"
#include <Arduino.h>
#include <util/atomic.h>
namespace {
volatile QuadratureState state = {};
uint8_t clk, dt;
uint8_t readAB() { return (digitalRead(clk) << 1) | digitalRead(dt); }
void onEdge() { quadratureUpdate(state, readAB()); }
}
bool encoderInit(uint8_t clkPin, uint8_t dtPin) {
  if (clkPin == dtPin || digitalPinToInterrupt(clkPin) == NOT_AN_INTERRUPT ||
      digitalPinToInterrupt(dtPin) == NOT_AN_INTERRUPT) return false;
  clk=clkPin; dt=dtPin;
  pinMode(clk, INPUT_PULLUP); pinMode(dt, INPUT_PULLUP);
  state.previous=readAB(); state.partial=0; state.position=0; state.invalidTransitions=0;
  attachInterrupt(digitalPinToInterrupt(clk), onEdge, CHANGE);
  attachInterrupt(digitalPinToInterrupt(dt), onEdge, CHANGE);
  return true;
}
EncoderReading encoderRead() {
  EncoderReading r;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    r.position=state.position; r.invalidTransitions=state.invalidTransitions;
  }
  return r;
}
