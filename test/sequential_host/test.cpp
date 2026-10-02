#include <Arduino.h>
#include <assert.h>
#include <stdio.h>
#include "app_lab_2.1.h"
#include "dd_button.h"
#include "sequentialScheduler.h"
uint32_t testTime = 0;
int testPins[70] = {};
void serialStdioInit(unsigned long) {}
void advance(uint32_t duration) {
  const uint32_t start = testTime;
  while (uint32_t(testTime - start) < duration) {
    lab21Loop();
    assert(testPins[systemSignals.greenConstant ? 3 : 2] == HIGH);
    ++testTime;
  }
}
void press(int pin) {
  testPins[pin] = HIGH; advance(100);
  testPins[pin] = LOW; advance(100);
}
int runs = 0;
void countRun(uint32_t) { ++runs; }
int main() {
  Button b;
  buttonInit(b, 2);
  testPins[2] = LOW; buttonUpdate(b, 1);
  testPins[2] = HIGH; buttonUpdate(b, 5);
  testPins[2] = LOW; buttonUpdate(b, 10);
  buttonUpdate(b, 39); assert(!buttonWasPressed(b));
  buttonUpdate(b, 40); assert(buttonWasPressed(b));
  assert(!buttonWasPressed(b));
  buttonUpdate(b, 400); assert(!buttonWasPressed(b));
  SequentialTask task = {countRun, 20, 5, 0};
  schedulerInit(&task, 1, 0xfffffff0U);
  assert(!schedulerRun(&task, 1, 0xfffffff4U));
  assert(schedulerRun(&task, 1, 0xfffffff5U));
  assert(!schedulerRun(&task, 1, 8));
  assert(schedulerRun(&task, 1, 9));
  assert(schedulerRun(&task, 1, 100));
  assert(runs == 3); // Recurentele ratate nu se executa in rafala.
  assert(!schedulerRun(&task, 1, 100));
  lab21Setup();
  advance(20); assert(testPins[2] == HIGH);
  advance(500); assert(testPins[2] == LOW);
  advance(540); assert(testPins[2] == HIGH);
  press(22); assert(!systemSignals.greenConstant && testPins[2]);
  testPins[22] = HIGH; advance(100); assert(systemSignals.greenConstant);
  advance(500); assert(systemSignals.greenConstant);
  testPins[22] = LOW; advance(100);
  press(24); assert(systemSignals.onSteps == 26);
  press(26); assert(systemSignals.onSteps == 25);
  systemSignals.onSteps = 100; press(24); assert(systemSignals.onSteps == 100);
  systemSignals.onSteps = 1; press(26); assert(systemSignals.onSteps == 1);
  testPins[24] = testPins[26] = HIGH; advance(100); assert(systemSignals.onSteps == 1);
  testPins[24] = testPins[26] = LOW; advance(100);
  testTime = 0xfffffff0U; lab21Setup(); advance(1100);
  puts("PASS: debounce, hold, blink timing, constant LED and role switching, limits, simultaneous buttons, millis rollover");
}
