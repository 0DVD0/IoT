#ifndef APP_LAB_21_H
#define APP_LAB_21_H
#include <stdint.h>

// Semnale provider/consumer; toate accesarile au loc in main, fara ISR.
struct SystemSignals {
  bool led1On;
  bool led2On;
  uint16_t onSteps;
  bool greenConstant;
};
extern SystemSignals systemSignals;
void lab21Setup();
void lab21Loop();
#endif
