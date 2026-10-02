#include "encoderSignals.h"
void encoderProcess(EncoderProcessor &p, EncoderSignals &s, bool filtering,
                    uint16_t cyclesPerRevolution) {
  if (!cyclesPerRevolution) cyclesPerRevolution=1;
  s.rawDegrees = float(s.position) * (360.0f / cyclesPerRevolution);
  float counts = float(s.position);
  if (filtering) {
    if (!p.initialized) {
      for (uint8_t i=0;i<5;++i) p.history[i]=s.position;
      p.next=0; p.initialized=true;
    }
    p.history[p.next]=s.position; p.next=(p.next+1)%5;
    int32_t sorted[5];
    for (uint8_t i=0;i<5;++i) sorted[i]=p.history[i];
    for (uint8_t i=1;i<5;++i) {
      int32_t value=sorted[i]; int8_t j=i-1;
      while (j>=0 && sorted[j]>value) { sorted[j+1]=sorted[j]; --j; }
      sorted[j+1]=value;
    }
    counts=weightedUpdate(p.weighted, float(sorted[2]));
  }
  s.filteredDegrees=counts*(360.0f/cyclesPerRevolution);
  s.outputDegrees=filtering ? signalSaturate(s.filteredDegrees,-360,360,s.saturated)
                           : s.filteredDegrees;
  if (!filtering) s.saturated=false;
}
