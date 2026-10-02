#ifndef LIGHT_REPORT_H
#define LIGHT_REPORT_H
#include "lightSignals.h"
// stdout trebuie initializat; un singur task raporteaza.
void lightPrintReport(const LightSignals &signals, uint32_t nowMs,
                      uint32_t staleAfterMs = 500);
#endif
