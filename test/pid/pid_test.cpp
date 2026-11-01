#include "pidControl.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
int main(){
 PidControl p;
 assert(!p.tune(-1,0,0));assert(!p.tune(NAN,0,0));
 assert(p.tune(1,0,0));assert(p.update(50,20,0.2f)==30);
 assert(p.update(1000,0,0.2f)==100);assert(p.update(0,100,0.2f)==0);
 assert(p.tune(0,0,1));assert(p.update(10,5,1)==0);
 assert(p.update(100,5,1)==0); // no derivative kick on reference
 assert(p.update(100,4,1)==1);
 assert(p.tune(1,1,0));
 for(int i=0;i<1000;i++)assert(p.update(1000,0,0.2f)==100);
 assert(p.update(0,0,0.2f)==0); // no integral accumulation at saturation
 assert(p.update(NAN,0,1)==0);
 assert(p.update(100,0,0)==0);
 assert(p.tune(1,0.1f,0));
 float speed=0;
 for(int i=0;i<2000;i++){float u=p.update(100,speed,0.2f);assert(u>=0&&u<=100);speed+=0.2f*(u*2-speed)/2;}
 assert(fabs(speed-100)<1);
 puts("PID tests passed");
}
