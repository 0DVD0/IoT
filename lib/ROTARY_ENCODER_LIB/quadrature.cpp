#include "quadrature.h"
#include <limits.h>
void quadratureUpdate(volatile QuadratureState &s, uint8_t ab) {
  ab &= 3;
  const uint8_t old = s.previous;
  s.previous = ab;
  if ((old ^ ab) == 3) {
    ++s.invalidTransitions;
    s.partial = 0;
    return;
  }
  static const int8_t transitions[16] = {
    0,-1,1,0, 1,0,0,-1, -1,0,0,1, 0,1,-1,0
  };
  s.partial += transitions[(old << 2) | ab];
  if (s.partial >= 4) {
    if (s.position < INT32_MAX) ++s.position;
    s.partial = 0;
  } else if (s.partial <= -4) {
    if (s.position > INT32_MIN) --s.position;
    s.partial = 0;
  }
}
