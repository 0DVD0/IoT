#include "signalFilters.h"
uint16_t medianUpdate(MedianFilter5 &filter, uint16_t value) {
  if (!filter.initialized) {
    for (uint8_t i = 0; i < 5; ++i) filter.values[i] = value;
    filter.nextIndex = 0;
    filter.initialized = true;
  }
  filter.values[filter.nextIndex] = value;
  filter.nextIndex = (filter.nextIndex + 1) % 5;
  uint16_t sorted[5];
  for (uint8_t i = 0; i < 5; ++i) sorted[i] = filter.values[i];
  for (uint8_t i = 1; i < 5; ++i) {
    const uint16_t current = sorted[i];
    int8_t j = i - 1;
    while (j >= 0 && sorted[j] > current) {
      sorted[j + 1] = sorted[j];
      --j;
    }
    sorted[j + 1] = current;
  }
  return sorted[2];
}
float weightedUpdate(WeightedFilter3 &filter, float value) {
  if (!filter.initialized) {
    for (uint8_t i = 0; i < 3; ++i) filter.values[i] = value;
    filter.initialized = true;
  }
  filter.values[0] = filter.values[1];
  filter.values[1] = filter.values[2];
  filter.values[2] = value;
  return (filter.values[0] + 2 * filter.values[1] + 3 * filter.values[2]) / 6;
}
float signalSaturate(float value, float minimum, float maximum, bool &saturated) {
  saturated = value < minimum || value > maximum;
  return value < minimum ? minimum : (value > maximum ? maximum : value);
}
