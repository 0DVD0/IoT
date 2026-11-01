#pragma once
#include "hysteresis.h"
#include <stdint.h>
enum class ThermostatCommandType { none, help, status, setPoint, band, invalid, tooLong };
struct ThermostatCommand { ThermostatCommandType type; float value; };
ThermostatCommand thermostatPollCommand();
void thermostatPrintHelp();
void thermostatPrintStatus(const HeatingHysteresis &control, float temperature,
                           bool valid, unsigned long errors);
