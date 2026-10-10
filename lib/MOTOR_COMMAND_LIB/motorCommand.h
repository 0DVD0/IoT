#pragma once
#include <stdint.h>
enum class MotorAction : uint8_t { set, stop, maximum, increase, decrease, status, help, invalid, tooLong };
struct MotorCommand { MotorAction action; int16_t percent; };
MotorCommand motorParseCommand(const char *line);
