#include "pidControl.h"
#include <math.h>
bool PidControl::tune(float p,float i,float d) {
  if (!isfinite(p)||!isfinite(i)||!isfinite(d)||p<0||i<0||d<0||p>100||i>100||d>100) return false;
  kp=p;ki=i;kd=d;reset();return true;
}
void PidControl::reset() { integral=0;previous=0;initialized=false; }
float PidControl::update(float target,float measured,float dt) {
  if (!isfinite(target)||!isfinite(measured)||!isfinite(dt)||dt<=0) {reset();return 0;}
  const float error=target-measured;
  // Derivata pe masurare evita impulsul la schimbarea referintei.
  const float derivative=initialized ? -(measured-previous)/dt : 0;
  previous=measured;initialized=true;
  const float candidate=integral+ki*error*dt;
  const float trial=kp*error+candidate+kd*derivative;
  // Integrare conditionata: nu acumulam eroare spre saturatie.
  if ((trial>=0 && trial<=100)||(trial>100 && error<0)||(trial<0 && error>0)) integral=candidate;
  const float output=kp*error+integral+kd*derivative;
  return output<0 ? 0 : output>100 ? 100 : output;
}
