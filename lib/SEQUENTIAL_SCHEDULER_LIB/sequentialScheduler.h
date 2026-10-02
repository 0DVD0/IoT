#ifndef SEQUENTIAL_SCHEDULER_H
#define SEQUENTIAL_SCHEDULER_H
#include <stddef.h>
#include <stdint.h>

// Perioade nenule si perioade/ofseturi mai mici de 2^31 ms.
struct SequentialTask {
  void (*run)(uint32_t nowMs);
  uint32_t periodMs;
  uint32_t offsetMs;
  uint32_t nextRunMs;
};
void schedulerInit(SequentialTask *tasks, size_t count, uint32_t nowMs);
// Cooperativ: fiecare task eligibil ruleaza cel mult o data per apel.
// Recurentele ratate sunt sarite, nu executate in rafala.
bool schedulerRun(SequentialTask *tasks, size_t count, uint32_t nowMs);
#endif
