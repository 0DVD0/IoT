#include "quadrature.h"
#include <cassert>
#include <initializer_list>
#include <limits.h>
int main() {
  QuadratureState s = {}; s.previous=3;
  for (auto x : {1,0,2,3}) quadratureUpdate(s,x);
  assert(s.position==1);
  for (auto x : {2,0,1,3}) quadratureUpdate(s,x);
  assert(s.position==0);
  for (auto x : {1,3,1,3,1,0,2,3}) quadratureUpdate(s,x);
  assert(s.position==1);
  quadratureUpdate(s,0);
  assert(s.invalidTransitions==1 && s.partial==0);
  s.position=INT32_MAX; s.previous=3;
  for (auto x : {1,0,2,3}) quadratureUpdate(s,x);
  assert(s.position==INT32_MAX);
}
