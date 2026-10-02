#include "encoderSignals.h"
#include <cassert>
int main() {
  EncoderProcessor p={}; EncoderSignals s={};
  s.position=-20; encoderProcess(p,s,false,20);
  assert(s.rawDegrees==-360 && s.outputDegrees==-360);
  s.position=0; encoderProcess(p,s,true,20);
  s.position=100; encoderProcess(p,s,true,20);
  assert(s.filteredDegrees==0);
  for (int i=0;i<8;++i) encoderProcess(p,s,true,20);
  assert(s.outputDegrees==360 && s.saturated);
  s.position=-100;
  for (int i=0;i<8;++i) encoderProcess(p,s,true,20);
  assert(s.outputDegrees==-360 && s.saturated);
}
