#include "dd_relay.h"
#include <Arduino.h>
void relayInit(Relay &relay, uint8_t pin, bool activeLow) {
  relay = {pin, activeLow, false};
  digitalWrite(pin, activeLow ? HIGH : LOW);
  pinMode(pin, OUTPUT);
}
void relayWrite(Relay &relay, bool on) {
  digitalWrite(relay.pin, on ? (relay.activeLow ? LOW : HIGH)
                            : (relay.activeLow ? HIGH : LOW));
  relay.commandedOn = on;
}
bool relayIsOn(const Relay &relay) { return relay.commandedOn; }
