#ifndef DD_RELAY_H
#define DD_RELAY_H
#include <stdint.h>
struct Relay {
  uint8_t pin;
  bool activeLow;
  bool commandedOn;
};
void relayInit(Relay &relay, uint8_t pin, bool activeLow);
void relayWrite(Relay &relay, bool on);
// Starea comandata, nu confirmarea mecanica a contactelor.
bool relayIsOn(const Relay &relay);
#endif
