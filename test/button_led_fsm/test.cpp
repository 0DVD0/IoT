#include <assert.h>
#include <stdio.h>
#include <Arduino.h>
#include <buttonLedFsm.h>
#include <dd_button.h>
#include "app_lab_6.1.h"
uint32_t testTime = 0;
int testPins[70] = {};
void serialStdioInit(unsigned long) {}
void advance(uint32_t duration) {
  for (uint32_t i = 0; i < duration; ++i) { ++testTime; lab61Loop(); }
}
int main() {
  ButtonLedFsm f;
  f.reset(0);
  assert(!f.output(0));
  assert(f.update(1, true) == ButtonLedEvent::Press);
  assert(f.output(1));
  assert(f.update(10000, false) == ButtonLedEvent::None);
  assert(f.update(10001, false) == ButtonLedEvent::Timeout);
  assert(f.state() == ButtonLedState::Blink);
  assert(f.output(10500) && !f.output(10501) && f.output(11001));
  f.update(11002, true); assert(f.state() == ButtonLedState::Off);
  f.update(12000, true);
  assert(f.update(22000, true) == ButtonLedEvent::Press);
  assert(f.state() == ButtonLedState::Off); // Button wins over timeout.
  f.reset(0xfffffff0U); f.update(0xfffffff0U, true);
  assert(f.update(uint32_t(0xfffffff0U + 10000U), false) == ButtonLedEvent::Timeout);
  assert(!f.output(uint32_t(0xfffffff0U + 10500U)));
  lab61Setup(); assert(testPins[7] == LOW);
  testPins[22] = LOW; advance(10);
  testPins[22] = HIGH; advance(10);
  testPins[22] = LOW; advance(30); assert(testPins[7] == LOW);
  advance(1); assert(testPins[7] == HIGH);
  advance(9000); assert(testPins[7] == HIGH); // Hold does not toggle again.
  advance(1000); assert(testPins[7] == HIGH); // Enter BLINK.
  advance(500); assert(testPins[7] == LOW);
  advance(500); assert(testPins[7] == HIGH);
  testPins[22] = HIGH; advance(31);
  testPins[22] = LOW; advance(31); assert(testPins[7] == LOW);
  testPins[22] = HIGH; advance(31);
  testPins[22] = LOW; advance(31); assert(testPins[7] == HIGH);
  testPins[22] = HIGH; advance(31);
  testPins[22] = LOW; advance(31); assert(testPins[7] == LOW);
  puts("PASS: FSM, timeout, blink, priority, rollover, debounce, hold and application outputs");
}
