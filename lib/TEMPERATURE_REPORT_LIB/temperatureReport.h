#ifndef TEMPERATURE_REPORT_H
#define TEMPERATURE_REPORT_H
#include "temperatureSignals.h"
// Scrie pe stdout deja initializat. Un singur task foloseste stdout.
void temperaturePrintReport(const TemperatureSignals &signals, uint32_t nowMs,
                            uint32_t staleAfterMs = 500);
#endif
