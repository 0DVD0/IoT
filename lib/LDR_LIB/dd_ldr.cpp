#include "dd_ldr.h"
#include <Arduino.h>
#include <math.h>
void ldrInit(uint8_t pin) {
  pinMode(pin, INPUT);
  analogReference(DEFAULT);
}

bool ldrConvert(uint16_t rawAdc, float &lux, const LdrConfig &config) {
  lux = 0.0f;

  // La capetele scalei conversia in lux nu este fiabila.
  // Saturatia nu inseamna automat senzor defect.
  if (rawAdc == 0 || rawAdc >= 1023 ||
      !isfinite(config.gamma) || config.gamma <= 0 ||
      !isfinite(config.resistanceAt10Lux) || config.resistanceAt10Lux <= 0 ||
      !isfinite(config.dividerResistance) || config.dividerResistance <= 0) {
    return false;
  }

  // Modelul Wokwi:
  // VCC -> rezistor fix 10k -> AO -> LDR -> GND.
  //
  // Folosim 1024 conform modelului ADC din exemplul Wokwi.
  const float resistance =
    config.dividerResistance * (float)rawAdc
    / (1024.0f - rawAdc);

  lux = pow(
    config.resistanceAt10Lux * pow(10.0f, config.gamma)
      / resistance,
    1.0f / config.gamma
  );

  return !isnan(lux) && !isinf(lux) && lux >= 0.0f;
}

bool ldrRead(uint8_t pin, uint16_t &rawAdc, float &lux, const LdrConfig &config) {
  rawAdc = ldrReadAdc(pin);
  return ldrConvert(rawAdc, lux, config);
}

uint16_t ldrReadAdc(uint8_t pin) { return analogRead(pin); }
bool ldrVoltageToLux(float voltage, float &lux, const LdrConfig &config,
                     float referenceVoltage) {
  lux = 0;
  if (!isfinite(referenceVoltage) || referenceVoltage <= 0 ||
      !isfinite(voltage) || voltage <= 0 ||
      voltage >= referenceVoltage * 1023.0f / 1024.0f ||
      !isfinite(config.gamma) || config.gamma <= 0 ||
      !isfinite(config.resistanceAt10Lux) || config.resistanceAt10Lux <= 0 ||
      !isfinite(config.dividerResistance) || config.dividerResistance <= 0) return false;
  const float resistance = config.dividerResistance * voltage / (referenceVoltage - voltage);
  lux = pow(config.resistanceAt10Lux * pow(10.0f, config.gamma) / resistance,
            1.0f / config.gamma);
  return isfinite(lux) && lux >= 0;
}
