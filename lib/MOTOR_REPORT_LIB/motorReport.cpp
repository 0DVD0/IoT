#include "motorReport.h"
#include <stdio.h>
void motorPrintHelp() {
  printf("Comenzi: motor set [-100..100], motor stop, motor max, motor inc, motor dec, motor status, help\n");
}
void motorPrintReport(const MotorSnapshot &s) {
  printf("[MOTOR #%lu] %s\n",(unsigned long)s.commandNumber,s.message);
  printf("Setat=%d%% | aplicat=%d%% | PWM=%u/255 | sens memorat=%s | stare=%s\n",
    (int)s.target,(int)s.applied,(unsigned)s.pwm,s.direction>0 ? "INAINTE" : "INAPOI",
    s.waiting ? "PAUZA INVERSARE" : s.applied ? "COMANDAT PORNIT" : "COMANDAT OPRIT");
}
