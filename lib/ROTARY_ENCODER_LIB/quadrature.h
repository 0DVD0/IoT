#pragma once
#include <stdint.h>
// Un ciclu complet A/B = patru tranzitii. Starea se initializeaza cu {}.
struct QuadratureState {
  uint8_t previous;
  int8_t partial;
  int32_t position;
  uint32_t invalidTransitions;
};
void quadratureUpdate(volatile QuadratureState &state, uint8_t ab);
