#include "trafficFsm.h"
void TrafficFsm::reset(uint32_t now) {
  northPending_ = pedestrianPending_ = northAfterPedestrian_ = false;
  enter(TrafficPhase::Startup, now);
}
void TrafficFsm::enter(TrafficPhase phase, uint32_t now) {
  phase_ = phase; enteredAt_ = now;
  if (phase == TrafficPhase::NorthGreen) northPending_ = false;
  if (phase == TrafficPhase::Pedestrian) pedestrianPending_ = false;
}
void TrafficFsm::update(uint32_t now, bool n, bool p) {
  if (n && phase_ != TrafficPhase::NorthGreen) northPending_ = true;
  if (p && phase_ != TrafficPhase::Pedestrian) pedestrianPending_ = true;
  const uint32_t elapsed = now - enteredAt_;
  switch (phase_) {
    case TrafficPhase::Startup:
      if (elapsed >= timing_.clearance) enter(TrafficPhase::EastGreen, now);
      break;
    case TrafficPhase::EastGreen:
      if (elapsed >= timing_.eastMin && (northPending_ || pedestrianPending_))
        enter(TrafficPhase::EastYellow, now);
      break;
    case TrafficPhase::EastYellow:
      if (elapsed >= timing_.yellow) enter(TrafficPhase::ToNorth, now);
      break;
    case TrafficPhase::ToNorth:
      if (elapsed >= timing_.clearance) {
        northAfterPedestrian_ = true;
        enter(pedestrianPending_ ? TrafficPhase::Pedestrian :
              northPending_ ? TrafficPhase::NorthGreen : TrafficPhase::EastGreen, now);
      }
      break;
    case TrafficPhase::NorthGreen:
      if (elapsed >= timing_.northGreen) enter(TrafficPhase::NorthYellow, now);
      break;
    case TrafficPhase::NorthYellow:
      if (elapsed >= timing_.yellow) enter(TrafficPhase::ToEast, now);
      break;
    case TrafficPhase::ToEast:
      if (elapsed >= timing_.clearance) {
        northAfterPedestrian_ = false;
        enter(pedestrianPending_ ? TrafficPhase::Pedestrian : TrafficPhase::EastGreen, now);
      }
      break;
    case TrafficPhase::Pedestrian:
      if (elapsed >= timing_.pedestrian)
        enter(northAfterPedestrian_ && northPending_ ? TrafficPhase::NorthGreen : TrafficPhase::EastGreen, now);
      break;
  }
}
TrafficColor TrafficFsm::east() const {
  return phase_ == TrafficPhase::EastGreen ? TrafficColor::Green :
         phase_ == TrafficPhase::EastYellow ? TrafficColor::Yellow : TrafficColor::Red;
}
TrafficColor TrafficFsm::north() const {
  return phase_ == TrafficPhase::NorthGreen ? TrafficColor::Green :
         phase_ == TrafficPhase::NorthYellow ? TrafficColor::Yellow : TrafficColor::Red;
}
const char *TrafficFsm::name(TrafficPhase p) {
  switch (p) {
    case TrafficPhase::Startup: return "START_ROSU";
    case TrafficPhase::EastGreen: return "EST_VERDE";
    case TrafficPhase::EastYellow: return "EST_GALBEN";
    case TrafficPhase::ToNorth: return "ROSU_SPRE_NORD";
    case TrafficPhase::NorthGreen: return "NORD_VERDE";
    case TrafficPhase::NorthYellow: return "NORD_GALBEN";
    case TrafficPhase::ToEast: return "ROSU_SPRE_EST";
    case TrafficPhase::Pedestrian: return "PIETONI";
  }
  return "UNKNOWN";
}
