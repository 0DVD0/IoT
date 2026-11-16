#include <distanceProtocol.h>
#include <assert.h>
#include <stdio.h>
using namespace DistanceProtocol;
int main() {
 uint8_t frame[frameSize]; Sample s={1,Status::Valid,245,65535}, result={};
 encode(s,frame);
 assert(frame[0]==0xAA && frame[1]==6 && frame[4]==245 && frame[5]==0 && frame[6]==255 && frame[7]==255);
 assert(decode(frame,frameSize,result)==Error::None && result.sequence==65535 && result.millimeters==245);
 for(size_t i=0;i<frameSize;++i) for(unsigned bit=0;bit<8;++bit) {
   frame[i]^=1U<<bit;assert(decode(frame,frameSize,result)!=Error::None);frame[i]^=1U<<bit;
 }
 for(size_t n=0;n<frameSize;++n) assert(decode(frame,n,result)==Error::Length);
 for(unsigned status=0;status<4;++status) {
   s.status=static_cast<Status>(status);s.sequence=0;encode(s,frame);
   assert(decode(frame,frameSize,result)==Error::None && result.sequence==0);
 }
 s={1,Status::Valid,0,1};encode(s,frame);assert(decode(frame,frameSize,result)==Error::Payload);
 s={2,Status::Valid,100,1};encode(s,frame);assert(decode(frame,frameSize,result)==Error::Payload);
 puts("PASS: format, endian, status, sequence rollover, truncation, all single-bit errors, payload validation");
}
