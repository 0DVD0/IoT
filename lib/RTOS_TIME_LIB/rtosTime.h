#ifndef RTOS_TIME_H
#define RTOS_TIME_H
#include <Arduino_FreeRTOS.h>
#include <stdint.h>

// Rotunjeste in sus, minimum un tick. 10 ms devin un tick (~16 ms)
// cu configuratia AVR implicita; nu reprezinta o perioada exacta de 10 ms.
inline TickType_t rtosTicksForMs(uint16_t durationMs) {
  const uint32_t ticks =
      (static_cast<uint32_t>(durationMs) * configTICK_RATE_HZ + 999UL) / 1000UL;
  return static_cast<TickType_t>(ticks ? ticks : 1);
}
// Conversie dupa durata nominala AVR, identica sketch-ului de senzori.
// 100/500/50 ms -> 7/32/4 tick-uri pentru un tick nominal de 16 ms.
inline TickType_t rtosNominalTicksForMs(uint16_t durationMs) {
  const uint32_t ticks = (static_cast<uint32_t>(durationMs)
      + portTICK_PERIOD_MS - 1UL) / portTICK_PERIOD_MS;
  return static_cast<TickType_t>(ticks ? ticks : 1);
}
#endif
