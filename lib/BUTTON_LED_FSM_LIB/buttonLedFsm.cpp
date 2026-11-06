#include "buttonLedFsm.h"

ButtonLedFsm::ButtonLedFsm(uint32_t onTimeoutMs, uint32_t blinkHalfPeriodMs)
    : onTimeoutMs_(onTimeoutMs),
      blinkHalfPeriodMs_(blinkHalfPeriodMs ? blinkHalfPeriodMs : 1) {}

void ButtonLedFsm::reset(uint32_t nowMs) {
  state_ = ButtonLedState::Off;
  enteredAtMs_ = nowMs;
}

ButtonLedEvent ButtonLedFsm::update(uint32_t nowMs, bool pressed) {
  ButtonLedEvent event = ButtonLedEvent::None;
  if (pressed) {
    state_ = state_ == ButtonLedState::Off ? ButtonLedState::On : ButtonLedState::Off;
    event = ButtonLedEvent::Press;
  } else if (state_ == ButtonLedState::On &&
             uint32_t(nowMs - enteredAtMs_) >= onTimeoutMs_) {
    state_ = ButtonLedState::Blink;
    event = ButtonLedEvent::Timeout;
  }
  if (event != ButtonLedEvent::None) enteredAtMs_ = nowMs;
  return event;
}

bool ButtonLedFsm::output(uint32_t nowMs) const {
  if (state_ == ButtonLedState::On) return true;
  if (state_ == ButtonLedState::Off) return false;
  return (uint32_t(nowMs - enteredAtMs_) / blinkHalfPeriodMs_) % 2 == 0;
}

const char *ButtonLedFsm::stateName(ButtonLedState state) {
  switch (state) {
    case ButtonLedState::Off: return "OFF";
    case ButtonLedState::On: return "ON";
    case ButtonLedState::Blink: return "BLINK";
  }
  return "UNKNOWN";
}
