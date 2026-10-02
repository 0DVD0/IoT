#include "temperatureReport.h"
#include <stdio.h>
#include <math.h>

static void printTemperature(float temperatureC) {
  const long centiDegrees = lroundf(temperatureC * 100.0f);

  const unsigned long magnitude =
    centiDegrees < 0
      ? (unsigned long)(-centiDegrees)
      : (unsigned long)centiDegrees;

  printf(
    "%s%lu.%02lu",
    centiDegrees < 0 ? "-" : "",
    magnitude / 100UL,
    magnitude % 100UL
  );
}

void temperaturePrintReport(const TemperatureSignals &signals, uint32_t nowMs,
                            uint32_t staleAfterMs) {
  const uint32_t ageMs = nowMs - signals.sampledAtMs;

  // Prag demonstrativ de prospetime a datelor.
  const bool stale = ageMs > staleAfterMs;

  printf(
    "\n[LAB 3.1] t=%lu ms | proba=%lu\n",
    (unsigned long)nowMs,
    (unsigned long)signals.sampleNumber
  );

  printf(
    "ADC=%u/1023 | U=%u mV | T=",
    (unsigned int)signals.rawAdc,
    (unsigned int)signals.voltageMv
  );

  if (signals.valid) {
    printTemperature(signals.temperatureC);
    printf(" C\n");
  } else {
    printf("indisponibila\n");
  }

  const char *state;

  if (!signals.valid) {
    state = "EROARE_ADC";
  } else if (stale) {
    state = "DATE_VECHI";
  } else if (signals.alert) {
    state = "ALERTA_TEMPERATURA";
  } else {
    state = "OK";
  }

  printf(
    "Stare=%s | varsta=%lu ms | erori=%lu\n",
    state,
    (unsigned long)ageMs,
    (unsigned long)signals.errorCount
  );
}

