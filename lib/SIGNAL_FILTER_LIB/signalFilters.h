#ifndef SIGNAL_FILTERS_H
#define SIGNAL_FILTERS_H
#include <stdint.h>
// Initializare cu {}. Stare independenta pentru fiecare canal.
struct MedianFilter5 { uint16_t values[5]; uint8_t nextIndex; bool initialized; };
struct WeightedFilter3 { float values[3]; bool initialized; };
uint16_t medianUpdate(MedianFilter5 &filter, uint16_t value);
float weightedUpdate(WeightedFilter3 &filter, float value);
float signalSaturate(float value, float minimum, float maximum, bool &saturated);
#endif
