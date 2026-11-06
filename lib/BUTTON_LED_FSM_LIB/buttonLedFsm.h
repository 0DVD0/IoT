#ifndef BUTTON_LED_FSM_H
#define BUTTON_LED_FSM_H
#include <stdint.h>

enum class ButtonLedState : uint8_t { Off, On, Blink };
enum class ButtonLedEvent : uint8_t { None, Press, Timeout };

// Independenta de Arduino: timpul si evenimentul debounced vin de la aplicatie.
class ButtonLedFsm {
 public:
  ButtonLedFsm(uint32_t onTimeoutMs = 10000, uint32_t blinkHalfPeriodMs = 500);
  void reset(uint32_t nowMs);
  // Apasarea are prioritate fata de timeout; returneaza cauza tranzitiei.
  ButtonLedEvent update(uint32_t nowMs, bool pressed);
  ButtonLedState state() const { return state_; }
  bool output(uint32_t nowMs) const;
  static const char *stateName(ButtonLedState state);
 private:
  ButtonLedState state_ = ButtonLedState::Off;
  uint32_t enteredAtMs_ = 0;
  uint32_t onTimeoutMs_;
  uint32_t blinkHalfPeriodMs_;
};
#endif
