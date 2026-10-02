#ifndef NTC_CONDITION_REPORT_H
#define NTC_CONDITION_REPORT_H
#include "ntcConditioning.h"
// stdout initializat; un singur task foloseste raportarea.
void ntcConditionPrintReport(const NtcProcessedSignals &signals, uint32_t nowMs);
#endif
