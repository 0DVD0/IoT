#ifndef JOYSTICK_CONDITION_REPORT_H
#define JOYSTICK_CONDITION_REPORT_H
#include "joystickConditioning.h"
// stdout initializat; raportare dintr-un singur task.
void joystickConditionPrintReport(const JoystickProcessedSignals &signals, uint32_t nowMs);
#endif
