#include "ldrConditionReport.h"
#include <stdio.h>
#include <stdlib.h>
namespace {
void printNumber(float value) {
  char text[48];
  dtostrf(value, 1, 2, text);
  printf("%s", text);
}
}
void ldrConditionPrintReport(const LdrProcessedSignals &s, uint32_t nowMs) {
  const uint32_t age = nowMs - s.sampledAtMs;
  printf("\n[LDR 3.2] proba=%lu | varsta=%lu ms\n",
         (unsigned long)s.sampleNumber, (unsigned long)age);
  printf("ADC original=%u | intrare=%u | median=%u | ponderat=",
         (unsigned)s.originalAdc, (unsigned)s.inputAdc, (unsigned)s.medianAdc);
  printNumber(s.filteredAdc);
  printf("\nU filtrata="); printNumber(s.voltage);
  printf(" V | E bruta=");
  if (s.rawValid) printNumber(s.rawLux); else printf("INVALIDA");
  printf(" | E filtrata=");
  if (s.filteredValid) {
    printNumber(s.filteredLux);
    printf(" | E iesire="); printNumber(s.outputLux); printf(" lux estimati");
  } else printf("INVALIDA | E iesire=INDISPONIBILA");
  const char *status = !s.filteredValid ? "CONVERSIE_INVALIDA" :
      (age > 500 ? "DATE_VECHI" : (s.saturated ? "SATURAT" : "OK"));
  printf("\nLumina=%s", !s.filteredValid ? "NECUNOSCUTA" : (s.dark ? "SCAZUTA" : "SUFICIENTA"));
  printf("\nStare=%s | impuls_curent=%s | impulsuri=%lu | ADC_extreme=%lu | pierdute=%lu\n",
         status, s.injected ? "DA" : "NU", (unsigned long)s.injectedCount,
         (unsigned long)s.inputRailCount, (unsigned long)s.droppedCount);
}
