#pragma once
#include <stdint.h>
#include "signalFilters.h"
struct EncoderSignals {
  int32_t position;
  uint32_t number, sampledAtMs, invalidTransitions, presses, dropped;
  int8_t direction;
  bool pressed, saturated;
  float rawDegrees, filteredDegrees, outputDegrees;
};
struct EncoderProcessor {
  int32_t history[5];
  uint8_t next;
  bool initialized;
  WeightedFilter3 weighted;
};
// cyclesPerRevolution reprezinta ciclurile A/B complete pe rotatie.
void encoderProcess(EncoderProcessor &state, EncoderSignals &s,
                    bool filtering, uint16_t cyclesPerRevolution);
