#include "app_lab_2.1.h"
#include "dd_led.h"
#include "dd_button.h"
#include "sequentialScheduler.h"
#include "ser_s_stdio.h"
#include <Arduino.h>
#include <stdio.h>

SystemSignals systemSignals = {true, false, 25, true};

namespace {
const uint8_t greenLedPin = 3;
const uint8_t redLedPin = 2;
const uint32_t taskPeriodMs = 20;
const uint32_t offTimeMs = 500;
const uint16_t minSteps = 1;
const uint16_t maxSteps = 100;
Button toggleButton;
Button increaseButton;
Button decreaseButton;
bool blinkEnabled = false;
bool blinkOn = false;
uint32_t phaseStartedMs = 0;
uint32_t phaseDurationMs = 0;
uint32_t lastReportMs = 0;

// Task 1: provider pentru greenConstant; D22 inverseaza rolurile.
void taskButtonLed(uint32_t nowMs) {
  buttonUpdate(toggleButton, nowMs);
  if (buttonWasPressed(toggleButton)) {
    systemSignals.greenConstant = !systemSignals.greenConstant;
    blinkEnabled = false;
    systemSignals.led1On = systemSignals.greenConstant;
    systemSignals.led2On = !systemSignals.greenConstant;
    ledWrite(greenLedPin, systemSignals.led1On);
    ledWrite(redLedPin, systemSignals.led2On);
  }
}

// Task 3: provider pentru onSteps. Apasari simultane se anuleaza.
void taskStateVariable(uint32_t nowMs) {
  buttonUpdate(increaseButton, nowMs);
  buttonUpdate(decreaseButton, nowMs);
  const bool increase = buttonWasPressed(increaseButton);
  const bool decrease = buttonWasPressed(decreaseButton);
  if (increase && !decrease && systemSignals.onSteps < maxSteps) ++systemSignals.onSteps;
  if (decrease && !increase && systemSignals.onSteps > minSteps) --systemSignals.onSteps;
}

// Task 2: consumer pentru greenConstant/onSteps; provider pentru stari LED.
void taskBlinkLed(uint32_t nowMs) {
  if (!blinkEnabled) {
    blinkEnabled = true;
    blinkOn = true;
    phaseStartedMs = nowMs;
    phaseDurationMs = systemSignals.onSteps * taskPeriodMs;
  } else if (static_cast<uint32_t>(nowMs - phaseStartedMs) >= phaseDurationMs) {
    blinkOn = !blinkOn;
    phaseStartedMs = nowMs;
    // Reglajul se aplica la inceputul urmatoarei faze ON.
    phaseDurationMs = blinkOn
        ? systemSignals.onSteps * taskPeriodMs : offTimeMs;
  }
  systemSignals.led1On = systemSignals.greenConstant || blinkOn;
  systemSignals.led2On = !systemSignals.greenConstant || blinkOn;
  ledWrite(greenLedPin, systemSignals.led1On);
  ledWrite(redLedPin, systemSignals.led2On);
}

// Ordine: furnizor roluri -> furnizor durata -> consumator LED-uri.
SequentialTask tasks[] = {
  {taskButtonLed, taskPeriodMs, 0, 0},
  {taskStateVariable, taskPeriodMs, 5, 0},
  {taskBlinkLed, taskPeriodMs, 10, 0}
};
const size_t taskCount = sizeof(tasks) / sizeof(tasks[0]);

void taskIdle(uint32_t nowMs) {
  if (static_cast<uint32_t>(nowMs - lastReportMs) >= 1000) {
    lastReportMs = nowMs;
    printf("L1=%u L2=%u N=%u Ton=%ums\n",
           static_cast<unsigned>(systemSignals.led1On),
           static_cast<unsigned>(systemSignals.led2On),
           static_cast<unsigned>(systemSignals.onSteps),
           static_cast<unsigned>(systemSignals.onSteps * taskPeriodMs));
  }
  delay(1);
}
}

void lab21Setup() {
  ledInit(greenLedPin);
  ledInit(redLedPin);
  buttonInit(toggleButton, 7, 30, false);
  buttonInit(increaseButton, 6, 30, false);
  buttonInit(decreaseButton, 5, 30, false);
  systemSignals = {true, false, 25, true};
  blinkEnabled = false;
  blinkOn = false;
  ledOn(greenLedPin);
  serialStdioInit(9600);
  printf("D22: schimba roluri | D24: + | D28: -\n");
  const uint32_t nowMs = millis();
  lastReportMs = nowMs;
  schedulerInit(tasks, taskCount, nowMs);
}

void lab21Loop() {
  const uint32_t nowMs = millis();
  if (!schedulerRun(tasks, taskCount, nowMs)) taskIdle(nowMs);
}
