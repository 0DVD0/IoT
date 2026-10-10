#include "dd_l298.h"
#include <Arduino.h>
void l298Init(L298 &d, uint8_t en, uint8_t a, uint8_t b) {
  d={en,a,b};
  digitalWrite(en,LOW); pinMode(en,OUTPUT);
  digitalWrite(a,LOW); digitalWrite(b,LOW);
  pinMode(a,OUTPUT); pinMode(b,OUTPUT);
}
void l298Coast(L298 &d) {
  analogWrite(d.enablePin,0);
  digitalWrite(d.in1Pin,LOW); digitalWrite(d.in2Pin,LOW);
}
void l298Drive(L298 &d, bool forward, uint8_t pwm) {
  // Dezactiveaza puntea inainte de schimbarea intrarilor.
  analogWrite(d.enablePin,0);
  digitalWrite(d.in1Pin,forward ? HIGH : LOW);
  digitalWrite(d.in2Pin,forward ? LOW : HIGH);
  analogWrite(d.enablePin,pwm);
}
