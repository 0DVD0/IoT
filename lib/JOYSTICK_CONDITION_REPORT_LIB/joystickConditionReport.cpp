#include "joystickConditionReport.h"
#include <stdio.h>
#include <stdlib.h>
namespace {
void printNumber(float value) {
  char text[48];
  dtostrf(value, 1, 2, text);
  printf("%s", text);
}
void printAxis(const char *name, const JoystickAxisSignals &s) {
  printf("%s: ADC original=%u | intrare=%u | median=%u | ponderat=",
         name, (unsigned)s.originalAdc, (unsigned)s.inputAdc, (unsigned)s.medianAdc);
  printNumber(s.filteredAdc);
  printf("\n   U="); printNumber(s.voltage);
  printf(" V | brut="); printNumber(s.rawPercent);
  printf("%% | filtrat="); printNumber(s.filteredPercent);
  printf("%% | iesire="); printNumber(s.outputPercent);
  printf("%% | saturat=%s\n", s.saturated ? "DA" : "NU");
}
}
void joystickConditionPrintReport(const JoystickProcessedSignals &s, uint32_t nowMs) {
  const uint32_t age = nowMs - s.sampledAtMs;
  printf("\n[JOYSTICK 3.2] proba=%lu | varsta=%lu ms\n",
         (unsigned long)s.sampleNumber, (unsigned long)age);
  printAxis("X", s.x); printAxis("Y", s.y);
  printf("Directie=%s / %s\n",
         s.x.outputPercent > 0 ? "DREAPTA" : (s.x.outputPercent < 0 ? "STANGA" : "CENTRU"),
         s.y.outputPercent > 0 ? "SUS" : (s.y.outputPercent < 0 ? "JOS" : "CENTRU"));
  printf("Buton=%s | apasari=%lu | stare=%s\n", s.buttonPressed ? "APASAT" : "ELIBERAT",
         (unsigned long)s.pressCount, age > 500 ? "DATE_VECHI" : "DATE_RECENTE");
  printf("Impuls_curent=%s | impulsuri=%lu | pierdute=%lu\n", s.injected ? "DA" : "NU",
         (unsigned long)s.injectedCount, (unsigned long)s.droppedCount);
}
