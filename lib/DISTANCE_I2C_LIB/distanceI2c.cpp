#include "distanceI2c.h"
#include <Arduino.h>
#include <Wire.h>
#include <util/atomic.h>
namespace {
uint8_t published[DistanceProtocol::frameSize];
volatile bool selected=false;
void receiveCommand(int count) {
  selected=count==1;
  if(Wire.available()) selected=(Wire.read()==DistanceProtocol::command)&&selected;
  else selected=false;
  while(Wire.available()) Wire.read();
}
void sendFrame() {
  if(selected) Wire.write(published,sizeof(published));
  selected=false;
}
}
void distanceSlavePublish(const DistanceProtocol::Sample &sample) {
  uint8_t next[DistanceProtocol::frameSize];DistanceProtocol::encode(sample,next);
  // AVR ISR cannot wait on a mutex. Copy only 9 bytes with interrupts masked.
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    for(size_t i=0;i<sizeof(next);++i) published[i]=next[i];
  }
}
void distanceSlaveBegin() {
  distanceSlavePublish({1,DistanceProtocol::Status::NoSample,0,0});
  selected=false;Wire.begin(DistanceProtocol::address);
  Wire.onReceive(receiveCommand);Wire.onRequest(sendFrame);
}
void distanceMasterBegin() {
  Wire.begin();Wire.setClock(100000);
#ifdef WIRE_HAS_TIMEOUT
  Wire.setWireTimeout(25000,true);
#endif
}
uint8_t distanceMasterRead(DistanceProtocol::Sample &sample,DistanceProtocol::Error &error) {
  error=DistanceProtocol::Error::None;
#ifdef WIRE_HAS_TIMEOUT
  Wire.clearWireTimeoutFlag();
#endif
  Wire.beginTransmission(DistanceProtocol::address);Wire.write(DistanceProtocol::command);
  uint8_t result=Wire.endTransmission();if(result) return result;
  uint8_t frame[DistanceProtocol::frameSize];size_t used=0;
  Wire.requestFrom(DistanceProtocol::address,static_cast<uint8_t>(sizeof(frame)));
  while(Wire.available()) {int b=Wire.read();if(used<sizeof(frame)) frame[used++]=b;}
#ifdef WIRE_HAS_TIMEOUT
  if(Wire.getWireTimeoutFlag()) return 5;
#endif
  error=DistanceProtocol::decode(frame,used,sample);return 0;
}
