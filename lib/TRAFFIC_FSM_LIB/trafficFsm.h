#pragma once
#include <stdint.h>
#include <trafficLight.h>
enum class TrafficPhase : uint8_t {
  Startup, EastGreen, EastYellow, ToNorth, NorthGreen, NorthYellow, ToEast, Pedestrian
};
// All durations and update timestamps use the same unit (ticks in the RTOS app).
struct TrafficTiming { uint32_t eastMin, northGreen, yellow, clearance, pedestrian; };
class TrafficFsm {
 public:
  explicit TrafficFsm(TrafficTiming timing) : timing_(timing) {}
  void reset(uint32_t now);
  void update(uint32_t now, bool northRequest, bool pedestrianRequest);
  TrafficPhase phase() const { return phase_; }
  bool northPending() const { return northPending_; }
  bool pedestrianPending() const { return pedestrianPending_; }
  TrafficColor east() const;
  TrafficColor north() const;
  static const char *name(TrafficPhase phase);
 private:
  void enter(TrafficPhase phase, uint32_t now);
  TrafficTiming timing_;
  TrafficPhase phase_ = TrafficPhase::Startup;
  uint32_t enteredAt_ = 0;
  bool northPending_ = false, pedestrianPending_ = false, northAfterPedestrian_ = false;
};
