#include "rtosSerialStdio.h"
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <stdio.h>
namespace {
FILE input,output;
bool skipLf=false;
int putCharacter(char c,FILE *) {
  if (c=='\n') Serial.write('\r');
  Serial.write((uint8_t)c); return 0;
}
int getCharacter(FILE *) {
  for (;;) {
    // Blocheaza task-ul, nu ocupa procesorul in asteptarea tastaturii.
    while (!Serial.available()) vTaskDelay(1);
    int c=Serial.read();
    if (skipLf && c=='\n') { skipLf=false; continue; }
    skipLf=c=='\r';
    return c=='\r' ? '\n' : c;
  }
}
}
void rtosSerialStdioInit(unsigned long baud) {
  Serial.begin(baud); skipLf=false;
  fdev_setup_stream(&input,NULL,getCharacter,_FDEV_SETUP_READ);
  fdev_setup_stream(&output,putCharacter,NULL,_FDEV_SETUP_WRITE);
  stdin=&input; stdout=&output;
}
