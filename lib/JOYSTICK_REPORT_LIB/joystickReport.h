#ifndef JOYSTICK_REPORT_H
#define JOYSTICK_REPORT_H
#include "joystickSignals.h"
// stdout deja initializat; raportarea se face dintr-un singur task.
void joystickPrintReport(const JoystickSignals &signals, uint32_t nowMs,
                         uint32_t staleAfterMs = 500);
#endif
