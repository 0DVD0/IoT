#include "distanceProtocol.h"
namespace DistanceProtocol {
static uint8_t checksum(const uint8_t *f) {
  uint8_t sum=0; for(size_t i=0;i<frameSize-1;++i) sum+=f[i]; return sum;
}
void encode(const Sample &s, uint8_t *f) {
  f[0]=head; f[1]=6; f[2]=s.sensor; f[3]=static_cast<uint8_t>(s.status);
  f[4]=s.millimeters&255; f[5]=s.millimeters>>8;
  f[6]=s.sequence&255; f[7]=s.sequence>>8; f[8]=checksum(f);
}
Error decode(const uint8_t *f,size_t length,Sample &s) {
  if(length!=frameSize) return Error::Length;
  if(f[0]!=head) return Error::Header;
  if(f[1]!=6) return Error::Length;
  if(f[8]!=checksum(f)) return Error::Checksum;
  const uint16_t mm=uint16_t(f[4])|(uint16_t(f[5])<<8);
  if(f[2]!=1 || f[3]>static_cast<uint8_t>(Status::OutOfRange) ||
     (f[3]==0 && (mm<20 || mm>4000))) return Error::Payload;
  s={f[2],static_cast<Status>(f[3]),mm,uint16_t(f[6]|(uint16_t(f[7])<<8))};
  return Error::None;
}
const char *errorName(Error e) {
  switch(e) {case Error::None:return "OK";case Error::Length:return "LUNGIME";
    case Error::Header:return "HEAD";case Error::Checksum:return "CHECKSUM";default:return "PAYLOAD";}
}
const char *statusName(Status s) {
  switch(s) {case Status::Valid:return "OK";case Status::NoSample:return "FARA_PROBA";
    case Status::EchoTimeout:return "TIMEOUT_ECHO";default:return "DISTANTA_INVALIDA";}
}
}
