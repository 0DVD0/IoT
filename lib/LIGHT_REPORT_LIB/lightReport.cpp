#include "lightReport.h"
#include <stdio.h>
#include <stdlib.h>

void lightPrintReport(const LightSignals &signals, uint32_t nowMs,
                      uint32_t staleAfterMs) {
  const uint32_t ageMs = nowMs - signals.sampledAtMs;

  printf(
    "\n[LDR] t=%lu ms | proba=%lu\n",
    (unsigned long)nowMs,
    (unsigned long)signals.sampleNumber
  );

  printf(
    "ADC=%u/1023 | U=%u mV | E=",
    (unsigned int)signals.rawAdc,
    (unsigned int)signals.voltageMv
  );

  if (signals.valid) {
    // printf AVR nu include implicit suport pentru %f.
    // Convertim numarul intr-un sir cu o zecimala.
    char luxText[48];
    dtostrf(signals.lux, 1, 1, luxText);
    printf("%s lux estimati\n", luxText);
  } else {
    printf("indisponibila\n");
  }

  const char *state;

  if (!signals.valid) {
    state = "SATURATIE/CONVERSIE_INVALIDA";
  } else if (ageMs > staleAfterMs) {
    state = "DATE_VECHI";
  } else if (signals.dark) {
    state = "LUMINA_SCAZUTA";
  } else {
    state = "LUMINA_OK";
  }

  printf(
    "Stare=%s | varsta=%lu ms | citiri_invalide=%lu\n",
    state,
    (unsigned long)ageMs,
    (unsigned long)signals.invalidCount
  );
}

