#include "app_lab_5.2.h"
#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dd_l298.h"
#include "dd_encoder.h"
#include "ser_s_stdio.h"

namespace {
const uint8_t enablePin=5, in1Pin=6, in2Pin=7;
const uint8_t encoderA=2, encoderB=3;
const int pwmStep=5;
const int encoderDirection=1; // Schimbati in -1 pentru sensul opus al butonului.
L298 motor;
uint8_t pwm=0;
bool encoderReady=false;
int32_t previousPosition=0;
char line[40];
size_t length=0;
bool overflow=false,previousCr=false;

void report() {
  printf("[5.2 TEST MANUAL] PWM=%u/255 | %s\n",
    (unsigned)pwm,pwm ? "COMANDAT PORNIT" : "OPRIT");
}
void applyPwm(uint8_t value) {
  pwm=value;
  if(pwm) l298Drive(motor,true,pwm);
  else l298Coast(motor);
}
void help() {
  printf("Encoder: +/-5 PWM pe ciclu complet, limite 0..255.\n");
  printf("Comenzi: stop | status | pwm 0..255 | help\n");
}
void command() {
  char name[12],argument[16],extra[2];
  const int count=sscanf(line,"%11s %15s %1s",name,argument,extra);
  if(count<1)return;
  if(count==1 && !strcmp(name,"stop")) {
    applyPwm(0);
    // Elimina miscarea acumulata inainte de comanda stop.
    previousPosition=encoderRead().position;
    report();return;
  }
  if(count==1 && !strcmp(name,"status")){report();return;}
  if(count==1 && !strcmp(name,"help")){help();return;}
  if(count==2 && !strcmp(name,"pwm")) {
    char *end;
    const long value=strtol(argument,&end,10);
    if(end!=argument && !*end && value>=0 && value<=255) {
      applyPwm((uint8_t)value);report();return;
    }
  }
  printf("EROARE: comanda invalida. Scrie help.\n");
}
void serialPoll() {
  for(uint8_t i=0;i<32;++i) {
    const int c=getchar();
    if(c==EOF){clearerr(stdin);return;}
    if(c=='\r'||c=='\n') {
      if(c=='\n' && previousCr){previousCr=false;continue;}
      previousCr=c=='\r';line[length]=0;
      if(overflow)printf("EROARE: linie prea lunga.\n");
      else command();
      length=0;overflow=false;return;
    }
    previousCr=false;
    if(overflow)continue;
    if(c==8||c==127){if(length)--length;continue;}
    if(c<32)continue;
    if(length<sizeof(line)-1)line[length++]=(char)c;
    else overflow=true;
  }
}
}

void lab52Setup() {
  l298Init(motor,enablePin,in1Pin,in2Pin);
  serialStdioInitNonBlocking(115200);
  encoderReady=encoderInit(encoderA,encoderB);
  previousPosition=encoderRead().position;
  printf("\nLab 5.2 - TEST MANUAL encoder -> PWM, fara PID sau masurare RPM\n");
  printf("L293D: EN=D5 IN1=D6 IN2=D7 | KY040: CLK=D2 DT=D3\n");
  printf("D18 si SW nu sunt utilizate. Motor oprit la pornire.\n");
  if(!encoderReady)printf("EROARE initializare encoder. Comenzile Serial raman disponibile.\n");
  help();report();
}
void lab52Loop() {
  if(encoderReady) {
    const int32_t position=encoderRead().position;
    const int64_t delta=(int64_t)position-previousPosition;
    previousPosition=position;
    if(delta) {
      const int64_t requested=(int64_t)pwm+delta*pwmStep*encoderDirection;
      const uint8_t next=requested<0 ? 0 : requested>255 ? 255 : (uint8_t)requested;
      if(next!=pwm){applyPwm(next);report();}
    }
  }
  // Procesam stop dupa modificarile encoderului din aceasta iteratie.
  serialPoll();
}
