#include "dd_ntc.h"
#include <Arduino.h>
#include <math.h>

void ntcInit(uint8_t pin) {
  pinMode(pin, INPUT);

  // Pe Mega alimentat la 5V, referinta implicita este AVcc.
  analogReference(DEFAULT);
}

bool ntcConvert(uint16_t rawAdc, float &temperatureC, float beta) {
  temperatureC = 0.0f;

  // Valorile de la capetele ADC nu permit aceasta conversie.
  // Pot indica saturatie sau o problema de conectare.
  if (rawAdc == 0 || rawAdc >= 1023 || beta <= 0.0f || isnan(beta) || isinf(beta)) {
    return false;
  }

  // Raportul R_NTC / R_25 pentru modulul NTC din Wokwi.
  const float resistanceRatio =
    (float)rawAdc / (1023.0f - rawAdc);

  const float inverseTemperature =
    log(resistanceRatio) / beta + 1.0f / 298.15f;

  if (inverseTemperature <= 0.0f) {
    return false;
  }

  temperatureC = 1.0f / inverseTemperature - 273.15f;

  return !isnan(temperatureC) && !isinf(temperatureC);
}

bool ntcRead(uint8_t pin, uint16_t &rawAdc, float &temperatureC, float beta) {
  rawAdc = ntcReadAdc(pin);
  return ntcConvert(rawAdc, temperatureC, beta);
}

uint16_t ntcReadAdc(uint8_t pin) { return analogRead(pin); }

bool ntcVoltageToTemperature(float voltage, float &temperatureC,
    float referenceVoltage, float fixedResistance, float referenceResistance,
    float beta, float referenceTemperatureK) {
  temperatureC = 0;
  if (!isfinite(voltage) || !isfinite(referenceVoltage) ||
      !isfinite(fixedResistance) || !isfinite(referenceResistance) ||
      !isfinite(beta) || !isfinite(referenceTemperatureK) ||
      voltage <= 0 || voltage >= referenceVoltage || fixedResistance <= 0 ||
      referenceResistance <= 0 || beta <= 0 || referenceTemperatureK <= 0) return false;
  const float resistance = fixedResistance * voltage / (referenceVoltage - voltage);
  const float inverseTemperature = 1.0f / referenceTemperatureK +
      log(resistance / referenceResistance) / beta;
  if (!isfinite(inverseTemperature) || inverseTemperature <= 0) return false;
  temperatureC = 1.0f / inverseTemperature - 273.15f;
  return isfinite(temperatureC);
}
