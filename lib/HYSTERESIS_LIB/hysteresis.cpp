#include "hysteresis.h"
#include <math.h>
HeatingHysteresis::HeatingHysteresis(float reference, float totalBand)
  : setPoint(0), width(1), on(false) { configure(reference, totalBand); }
bool HeatingHysteresis::configure(float reference, float totalBand) {
  if (!isfinite(reference) || !isfinite(totalBand) || totalBand <= 0 ||
      !isfinite(reference - totalBand / 2) ||
      !isfinite(reference + totalBand / 2)) return false;
  setPoint = reference;
  width = totalBand;
  return true;
}
bool HeatingHysteresis::update(float actual, bool valid) {
  if (!valid || !isfinite(actual)) on = false;
  else if (actual < lower()) on = true;
  else if (actual > upper()) on = false;
  // Egalitate sau interiorul benzii: memoram starea anterioara.
  return on;
}
