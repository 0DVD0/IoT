#include "hysteresis.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
int main() {
  HeatingHysteresis c(28, 2);
  assert(!c.update(28, true));
  assert(c.update(26, true));
  assert(c.update(27, true));
  assert(c.update(28, true));
  assert(c.update(29, true));
  assert(!c.update(30, true));
  assert(!c.update(29, true));
  assert(!c.update(27, true));
  assert(c.update(26, true));
  assert(!c.update(26, false));
  assert(!c.update(28, true));
  assert(c.update(26, true));
  assert(!c.update(NAN, true));
  assert(!c.configure(28, 0));
  assert(!c.configure(NAN, 2));
  assert(c.reference() == 28 && c.band() == 2);
  assert(c.configure(30, 4));
  assert(c.lower() == 28 && c.upper() == 32);
  assert(c.update(27, true));
  assert(!c.update(33, true));
  puts("Hysteresis tests passed");
}
