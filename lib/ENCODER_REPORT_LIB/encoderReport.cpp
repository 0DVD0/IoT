#include "encoderReport.h"
#include <stdio.h>
#include <stdlib.h>
void encoderPrintReport(const EncoderSignals &s, uint32_t nowMs) {
  char raw[32], filtered[32], output[32];
  dtostrf(s.rawDegrees,1,1,raw); dtostrf(s.filteredDegrees,1,1,filtered);
  dtostrf(s.outputDegrees,1,1,output);
  printf("\n[ENCODER] t=%lu proba=%lu pozitie=%ld sens=%s\n",
    (unsigned long)nowMs,(unsigned long)s.number,(long)s.position,
    s.direction>0 ? "PLUS" : s.direction<0 ? "MINUS" : "STOP");
  printf("Unghi relativ: brut=%s filtrat=%s iesire=%s grade saturat=%u\n",
    raw,filtered,output,(unsigned)s.saturated);
  printf("Buton=%u apasari=%lu tranzitii invalide=%lu pierdute=%lu varsta=%lu ms\n",
    (unsigned)s.pressed,(unsigned long)s.presses,(unsigned long)s.invalidTransitions,
    (unsigned long)s.dropped,(unsigned long)(nowMs-s.sampledAtMs));
}
