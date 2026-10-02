#ifndef DD_NTC_H
#define DD_NTC_H
#include <stdint.h>

// Model AVR ADC 10 biti, divizor cu Rfix = R_NTC la 25 C.
// Raport: R_NTC/R25 = ADC/(1023-ADC), ca in modulul Wokwi.
// Referinta ADC DEFAULT este comuna tuturor intrarilor analogice.
void ntcInit(uint8_t pin);
bool ntcConvert(uint16_t rawAdc, float &temperatureC, float beta = 3950.0f);
bool ntcRead(uint8_t pin, uint16_t &rawAdc, float &temperatureC,
             float beta = 3950.0f);
// Citire bruta si conversie explicita, pentru filtrare inainte de conversie.
uint16_t ntcReadAdc(uint8_t pin);
bool ntcVoltageToTemperature(float voltage, float &temperatureC,
    float referenceVoltage = 5.0f, float fixedResistance = 10000.0f,
    float referenceResistance = 10000.0f, float beta = 3950.0f,
    float referenceTemperatureK = 298.15f);
#endif
