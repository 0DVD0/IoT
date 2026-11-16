#include "app_lab_7.1_master.h"
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <stdio.h>
#include <distanceI2c.h>
#include <dd_led.h>
#include <rtosSerialStdio.h>
namespace {
constexpr uint8_t alertPin=7;
QueueHandle_t reports;
struct Report { DistanceProtocol::Sample sample; DistanceProtocol::Error protocol;uint8_t bus;bool stale,alert; };
TickType_t ticksFor(uint32_t ms) {return (ms+portTICK_PERIOD_MS-1)/portTICK_PERIOD_MS;}
void pollTask(void *) {
  bool seen=false,alert=false;uint16_t sequence=0;uint32_t changedAt=0;
  TickType_t wake=xTaskGetTickCount();
  for(;;) {
    Report r={};r.bus=distanceMasterRead(r.sample,r.protocol);
    const uint32_t now=millis();
    if(!r.bus && r.protocol==DistanceProtocol::Error::None) {
      if(!seen || r.sample.sequence!=sequence) {seen=true;sequence=r.sample.sequence;changedAt=now;}
      r.stale=uint32_t(now-changedAt)>=1500;
      if(r.sample.status==DistanceProtocol::Status::Valid && !r.stale) {
        if(r.sample.millimeters<100) alert=true;
        else if(r.sample.millimeters>120) alert=false;
      } else alert=false;
    } else alert=false;
    // LED means confirmed proximity only; faults are explicit in the report.
    r.alert=alert;ledWrite(alertPin,alert);xQueueOverwrite(reports,&r);
    vTaskDelayUntil(&wake,ticksFor(500));
  }
}
void reportTask(void *) {
  Report r;
  for(;;) {
    if(xQueueReceive(reports,&r,portMAX_DELAY)!=pdTRUE) continue;
    if(r.bus) printf("[I2C] EROARE cod=%u (2=NACK adresa, 5=timeout)\n",r.bus);
    else if(r.protocol!=DistanceProtocol::Error::None)
      printf("[PROTOCOL] EROARE %s\n",DistanceProtocol::errorName(r.protocol));
    else if(r.stale) printf("[SLAVE 0x12] DATE VECHI | proba=%u\n",r.sample.sequence);
    else if(r.sample.status!=DistanceProtocol::Status::Valid)
      printf("[SENZOR] %s | proba=%u\n",DistanceProtocol::statusName(r.sample.status),r.sample.sequence);
    else printf("[SLAVE 0x12] Senzor=%u | distanta=%u mm | proba=%u | %s\n",
      r.sample.sensor,r.sample.millimeters,r.sample.sequence,r.alert?"ALERTA APROPIERE":"OK");
  }
}
}
void lab71MasterSetup() {
  ledInit(alertPin);rtosSerialStdioInit(115200);distanceMasterBegin();
  printf("Lab 7.1 MASTER | SDA=D20 SCL=D21 | slave=0x12 | alerta D7\n");
  printf("Alerta <100 mm, oprire >120 mm | date vechi dupa 1500 ms fara proba noua\n");
  reports=xQueueCreate(1,sizeof(Report));
  if(!reports || xTaskCreate(pollTask,"I2CPoll",384,NULL,2,NULL)!=pdPASS ||
     xTaskCreate(reportTask,"DistanceUI",640,NULL,1,NULL)!=pdPASS) {
    printf("EROARE INIT TASKURI\n");Serial.flush();for(;;){}
  }
  Serial.flush();
}
void lab71MasterLoop() {}
