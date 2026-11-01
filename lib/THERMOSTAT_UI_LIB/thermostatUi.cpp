#include "thermostatUi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
namespace {
const size_t capacity = 64;
char line[capacity];
size_t length = 0;
bool overflow = false;
bool previousCr = false;
ThermostatCommand parse() {
  char name[16], argument[24], extra[2];
  const int count = sscanf(line, "%15s %23s %1s", name, argument, extra);
  if (count < 1) return {ThermostatCommandType::none, 0};
  if (count == 1 && !strcmp(name, "help")) return {ThermostatCommandType::help, 0};
  if (count == 1 && !strcmp(name, "status")) return {ThermostatCommandType::status, 0};
  const bool set = !strcmp(name, "setpoint");
  const bool band = !strcmp(name, "hysteresis");
  if (count != 2 || (!set && !band)) return {ThermostatCommandType::invalid, 0};
  char *end;
  const double value = strtod(argument, &end);
  if (end == argument || *end || !isfinite(value) ||
      (set && (value < 0 || value > 50)) ||
      (band && (value < 0.5 || value > 10)))
    return {ThermostatCommandType::invalid, 0};
  return {set ? ThermostatCommandType::setPoint : ThermostatCommandType::band,
          static_cast<float>(value)};
}
}
ThermostatCommand thermostatPollCommand() {
  // stdin trebuie configurat neblocant. Buget limitat pentru fiecare loop.
  for (uint8_t i = 0; i < 32; ++i) {
    const int c = getchar();
    if (c == EOF) { clearerr(stdin); break; }
    if (c == '\r' || c == '\n') {
      if (c == '\n' && previousCr) { previousCr = false; continue; }
      previousCr = c == '\r';
      line[length] = 0;
      const ThermostatCommand result = overflow
        ? ThermostatCommand{ThermostatCommandType::tooLong, 0} : parse();
      length = 0; overflow = false;
      return result;
    }
    previousCr = false;
    if (overflow) continue;
    if (c == '\b' || c == 127) { if (length) --length; continue; }
    if (c < 32) continue;
    if (length < capacity - 1) line[length++] = static_cast<char>(c);
    else overflow = true;
  }
  return {ThermostatCommandType::none, 0};
}
void thermostatPrintHelp() {
  printf("Comenzi: setpoint 28 (0..50 C), hysteresis 2 (0.5..10 C), status, help\n");
  printf("Histereza = latimea TOTALA. Zecimale cu punct; trimitere cu Enter.\n");
}
void thermostatPrintStatus(const HeatingHysteresis &control, float temperature,
                           bool valid, unsigned long errors) {
  char t[16], sp[16], h[16], low[16], high[16];
  if (valid) dtostrf(temperature, 1, 1, t);
  else strcpy(t, "indisponibila");
  dtostrf(control.reference(), 1, 1, sp);
  dtostrf(control.band(), 1, 1, h);
  dtostrf(control.lower(), 1, 1, low);
  dtostrf(control.upper(), 1, 1, high);
  printf("[LAB 5.1] Temperatura masurata=%s C | SP=%s C | H=%s C\n", t, sp, h);
  printf("Praguri=%s...%s C | RELEU=%s | SENZOR=%s | erori=%lu\n",
    low, high, control.isOn() ? "ON" : "OFF", valid ? "OK" : "EROARE", errors);
  if (!valid) printf("ALERTA: incalzire oprita; temperatura invalida sau veche.\n");
}
