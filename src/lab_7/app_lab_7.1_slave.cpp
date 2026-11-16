#include "app_lab_7.1_slave.h"
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <hcsr04.h>
#include <distanceI2c.h>
namespace {
QueueHandle_t samples;
TickType_t ticksFor(uint32_t ms) {return (ms+portTICK_PERIOD_MS-1)/portTICK_PERIOD_MS;}
void acquireTask(void *) {
  uint16_t sequence=0;TickType_t wake=xTaskGetTickCount();
  for(;;) {
    hcsr04Start();const uint32_t started=micros();uint32_t duration=0;bool received=false;
    do {
      received=hcsr04TakePulse(duration);
      if(received) break;
      vTaskDelay(1);
    } while(uint32_t(micros()-started)<40000UL);
    hcsr04Cancel();
    DistanceProtocol::Sample s={1,DistanceProtocol::Status::EchoTimeout,0,++sequence};
    if(received && duration<=30000UL) {
      const uint32_t mm=(duration*10UL+29)/58;
      s.status=mm>=20 && mm<=4000 ? DistanceProtocol::Status::Valid : DistanceProtocol::Status::OutOfRange;
      s.millimeters=static_cast<uint16_t>(mm);
    }
    xQueueOverwrite(samples,&s);
    vTaskDelayUntil(&wake,ticksFor(100));
  }
}
void publishTask(void *) {
  DistanceProtocol::Sample s;
  for(;;) if(xQueueReceive(samples,&s,portMAX_DELAY)==pdTRUE) distanceSlavePublish(s);
}
void fail() { pinMode(LED_BUILTIN,OUTPUT);digitalWrite(LED_BUILTIN,HIGH);for(;;){} }
}
void lab71SlaveSetup() {
  if(!hcsr04Init(6,2)) fail();
  distanceSlaveBegin();samples=xQueueCreate(1,sizeof(DistanceProtocol::Sample));
  if(!samples || xTaskCreate(acquireTask,"Distance",256,NULL,2,NULL)!=pdPASS ||
     xTaskCreate(publishTask,"I2CPublish",256,NULL,1,NULL)!=pdPASS) fail();
}
void lab71SlaveLoop() {}
