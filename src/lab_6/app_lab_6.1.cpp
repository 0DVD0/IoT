#include "app_lab_6.1.h"
#include <Arduino.h>
#include <stdio.h>
#include <dd_button.h>
#include <dd_led.h>
#include <ser_s_stdio.h>
#include <buttonLedFsm.h>

namespace {
constexpr uint8_t buttonPin = 22;
constexpr uint8_t ledPin = 7;
constexpr uint16_t debounceMs = 30;
constexpr uint32_t onTimeoutMs = 10000;
constexpr uint32_t blinkHalfPeriodMs = 500;
Button button;
ButtonLedFsm fsm(onTimeoutMs, blinkHalfPeriodMs);
bool ledOutput = false;
}

void lab61Setup() {
  ledInit(ledPin);
  buttonInit(button, buttonPin, debounceMs, true);
  fsm.reset(millis());
  ledOutput = false;
  serialStdioInit(115200);
  printf("Lab 6.1 | Buton=D22 -> GND (INPUT_PULLUP) | LED=D7\n");
  printf("Debounce=30 ms | ON -> BLINK dupa 10 s | clipire 500/500 ms\n");
  printf("[START] Stare=OFF | LED=OFF\n");
}

void lab61Loop() {
  const uint32_t nowMs = millis();
  buttonUpdate(button, nowMs);
  const ButtonLedState previous = fsm.state();
  const ButtonLedEvent event = fsm.update(nowMs, buttonWasPressed(button));
  const bool nextOutput = fsm.output(nowMs);
  if (nextOutput != ledOutput) {
    ledWrite(ledPin, nextOutput);
    ledOutput = nextOutput;
  }
  if (event != ButtonLedEvent::None) {
    printf("[FSM] t=%lu ms | %s | %s -> %s | LED=%s\n",
           static_cast<unsigned long>(nowMs),
           event == ButtonLedEvent::Press ? "APASARE" : "TIMEOUT",
           ButtonLedFsm::stateName(previous), ButtonLedFsm::stateName(fsm.state()),
           ledOutput ? "ON" : "OFF");
  }
}
