#include "sequentialScheduler.h"
void schedulerInit(SequentialTask *tasks, size_t count, uint32_t nowMs) {
  for (size_t i = 0; i < count; ++i) tasks[i].nextRunMs = nowMs + tasks[i].offsetMs;
}
bool schedulerRun(SequentialTask *tasks, size_t count, uint32_t nowMs) {
  bool ran = false;
  for (size_t i = 0; i < count; ++i) {
    SequentialTask &task = tasks[i];
    if (!task.run || task.periodMs == 0 ||
        static_cast<int32_t>(nowMs - task.nextRunMs) < 0) continue;
    const uint32_t lateMs = nowMs - task.nextRunMs;
    task.nextRunMs += (lateMs / task.periodMs + 1) * task.periodMs;
    task.run(nowMs);
    ran = true;
  }
  return ran;
}
