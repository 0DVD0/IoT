#include <assert.h>
#include <stdio.h>
#include <trafficFsm.h>
void safe(TrafficFsm &f) {
  assert(f.east() == TrafficColor::Red || f.north() == TrafficColor::Red);
  if (f.phase() == TrafficPhase::Pedestrian)
    assert(f.east() == TrafficColor::Red && f.north() == TrafficColor::Red);
}
int main() {
  TrafficFsm f({5000,5000,2000,1000,5000});
  f.reset(0); safe(f);
  f.update(999,false,false); assert(f.phase()==TrafficPhase::Startup);
  f.update(1000,false,false); assert(f.phase()==TrafficPhase::EastGreen);
  f.update(5000,true,false); assert(f.phase()==TrafficPhase::EastGreen && f.northPending());
  f.update(6000,true,false); assert(f.phase()==TrafficPhase::EastYellow);
  f.update(8000,false,false); assert(f.phase()==TrafficPhase::ToNorth);
  f.update(9000,false,false); assert(f.phase()==TrafficPhase::NorthGreen && !f.northPending());
  f.update(10000,true,false); assert(!f.northPending());
  f.update(14000,false,false); assert(f.phase()==TrafficPhase::NorthYellow);
  f.update(15000,true,true); assert(f.northPending() && f.pedestrianPending());
  f.update(16000,false,false); assert(f.phase()==TrafficPhase::ToEast);
  f.update(17000,false,false); assert(f.phase()==TrafficPhase::Pedestrian); safe(f);
  f.update(18000,false,true); assert(!f.pedestrianPending());
  f.update(22000,false,false); assert(f.phase()==TrafficPhase::EastGreen);
  f.update(26999,false,false); assert(f.phase()==TrafficPhase::EastGreen);
  f.update(27000,false,true); assert(f.phase()==TrafficPhase::EastYellow);
  f.update(29000,false,false); f.update(30000,false,false);
  assert(f.phase()==TrafficPhase::Pedestrian);
  f.update(35000,false,false); assert(f.phase()==TrafficPhase::NorthGreen);
  f.reset(0); f.update(1000,false,false); f.update(100000,false,false);
  assert(f.phase()==TrafficPhase::EastGreen);
  const uint32_t base=0xffffff00U;
  f.reset(base); f.update(base+1000U,true,false);
  assert(f.phase()==TrafficPhase::EastGreen);
  f.update(base+6000U,false,false); assert(f.phase()==TrafficPhase::EastYellow);
  // Dense requests, tick rollover, and every observable phase preserve safety.
  f.reset(base);
  uint32_t random=123;
  for(uint32_t t=0;t<300000;t+=10) {
    random=random*1664525U+1013904223U;
    f.update(base+t,random%17==0,random%31==0); safe(f);
  }
  puts("PASS: priority, minimum green, request coalescing, pedestrian routing, clearance, rollover, mutual exclusion");
}
