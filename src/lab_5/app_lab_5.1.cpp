#include "app_lab_5.1.h"
#include <Arduino.h>
#include "dd_dht.h"
#include "dd_relay.h"
#include "hysteresis.h"
#include "thermostatUi.h"
#include "ser_s_stdio.h"

namespace {
const uint8_t dhtPin = 22;
const uint8_t dhtType = DHT11; // Pentru modulul fizic KY-015: DHT11.
const uint8_t relayPin = 7;
const bool relayActiveLow = true; // Verificati polaritatea modulului fizic.
const uint8_t alarmPin = LED_BUILTIN;
const unsigned long samplePeriodMs = 2000;
const unsigned long reportPeriodMs = 2000;
const unsigned long staleTimeoutMs = 6000;
DhtTemperatureSensor sensor(dhtPin, dhtType);
Relay relay;
HeatingHysteresis control(28.0f, 2.0f);
float temperature = 0;
bool sensorValid = false;
unsigned long lastSample = 0, lastGoodSample = 0, lastReport = 0, errors = 0;
bool usable() {
  return sensorValid && (unsigned long)(millis() - lastGoodSample) <= staleTimeoutMs;
}
void updateControl() {
  const bool valid = usable();
  relayWrite(relay, control.update(temperature, valid));
  digitalWrite(alarmPin, valid ? LOW : HIGH);
}
void report() { thermostatPrintStatus(control, temperature, usable(), errors); }
void processCommand() {
  const ThermostatCommand command = thermostatPollCommand();
  switch (command.type) {
    case ThermostatCommandType::none: return;
    case ThermostatCommandType::help: thermostatPrintHelp(); return;
    case ThermostatCommandType::status: updateControl(); report(); return;
    case ThermostatCommandType::setPoint:
      control.configure(command.value, control.band()); break;
    case ThermostatCommandType::band:
      control.configure(control.reference(), command.value); break;
    case ThermostatCommandType::tooLong:
      printf("EROARE: comanda prea lunga; linie ignorata.\n"); return;
    default:
      printf("EROARE: comanda sau valoare invalida. Scrie help.\n"); return;
  }
  updateControl();
  printf("OK: configuratie actualizata.\n");
  report();
}
}
void lab51Setup() {
  relayInit(relay, relayPin, relayActiveLow);
  pinMode(alarmPin, OUTPUT);
  digitalWrite(alarmPin, LOW);
  serialStdioInitNonBlocking(115200);
  sensor.begin();
  printf("\nLab 5.1 - Temperatura ON-OFF cu histereza\n");
  printf("DHT%u: D22 | Releu: D7 | Serial: 115200\n", (unsigned)dhtType);
  printf("SP=28 C | H total=2 C | praguri=27...29 C\n");
  printf("Releu OFF pana la prima citire valida.\n");
  printf("Temperatura masurata este afisata automat la fiecare 2 secunde.\n");
  thermostatPrintHelp();
  lastSample = lastReport = millis();
}
void lab51Loop() {
  const unsigned long now = millis();
  if ((unsigned long)(now - lastSample) >= samplePeriodMs) {
    lastSample = now;
    sensorValid = sensor.read(temperature);
    if (sensorValid) lastGoodSample = millis();
    else ++errors;
  }
  updateControl();
  processCommand();
  if ((unsigned long)(now - lastReport) >= reportPeriodMs) {
    lastReport = now;
    report();
  }
}
