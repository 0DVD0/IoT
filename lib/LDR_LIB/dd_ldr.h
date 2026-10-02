#ifndef DD_LDR_H
#define DD_LDR_H
#include <stdint.h>
// Model ADC 10 biti: VCC -> rezistor fix -> AO -> LDR -> GND.
// Rezistente in ohmi. Lux estimati; calibrati pentru modulul fizic.
struct LdrConfig {
  float gamma = 0.7f;
  float resistanceAt10Lux = 50000.0f;
  float dividerResistance = 10000.0f;
};
void ldrInit(uint8_t pin);
bool ldrConvert(uint16_t rawAdc, float &lux, const LdrConfig &config);
bool ldrRead(uint8_t pin, uint16_t &rawAdc, float &lux, const LdrConfig &config);
uint16_t ldrReadAdc(uint8_t pin);
// Aceeasi scalare 10 biti ca in 3.1; exclude extremele ADC.
bool ldrVoltageToLux(float voltage, float &lux, const LdrConfig &config,
                     float referenceVoltage = 5.0f);
#endif
