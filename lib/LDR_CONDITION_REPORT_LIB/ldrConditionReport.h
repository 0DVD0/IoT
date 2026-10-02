#ifndef LDR_CONDITION_REPORT_H
#define LDR_CONDITION_REPORT_H
#include "ldrConditioning.h"
// stdout initializat; un singur task foloseste raportarea.
void ldrConditionPrintReport(const LdrProcessedSignals &signals, uint32_t nowMs);
#endif
