#include "joystickReport.h"
#include <stdio.h>

static const char *horizontalDirection(int16_t x) {
  if (x < 0) {
    return "STANGA";
  }

  if (x > 0) {
    return "DREAPTA";
  }

  return "CENTRU";
}

static const char *verticalDirection(int16_t y) {
  if (y < 0) {
    return "JOS";
  }

  if (y > 0) {
    return "SUS";
  }

  return "CENTRU";
}

void joystickPrintReport(const JoystickSignals &signals, uint32_t nowMs,
                         uint32_t staleAfterMs) {
  const uint32_t ageMs = nowMs - signals.sampledAtMs;

  printf(
    "\n[JOYSTICK] t=%lu ms | proba=%lu\n",
    (unsigned long)nowMs,
    (unsigned long)signals.sampleNumber
  );

  printf(
    "ADC: X=%u Y=%u | Pozitie: X=%d%% Y=%d%%\n",
    (unsigned int)signals.rawX,
    (unsigned int)signals.rawY,
    (int)signals.xPercent,
    (int)signals.yPercent
  );

  printf(
    "Directie: %s / %s\n",
    horizontalDirection(signals.xPercent),
    verticalDirection(signals.yPercent)
  );

  printf(
    "Buton=%s | apasari=%lu | varsta=%lu ms | stare=%s\n",
    signals.buttonPressed ? "APASAT" : "ELIBERAT",
    (unsigned long)signals.pressCount,
    (unsigned long)ageMs,
    ageMs > staleAfterMs ? "DATE_VECHI" : "DATE_RECENTE"
  );
}

