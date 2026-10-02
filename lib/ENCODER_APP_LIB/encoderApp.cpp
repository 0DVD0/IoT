#include "encoderApp.h"
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include "dd_encoder.h"
#include "dd_button.h"
#include "encoderSignals.h"
#include "encoderReport.h"
#include "ser_s_stdio.h"
#include "rtosTime.h"
namespace {
// Verificati numarul de cicluri COMPLETE A/B intr-o rotatie de 360 grade.
const uint16_t cyclesPerRevolution=20;
const uint16_t acquisitionMs=20;
bool filtering;
Button button;
QueueHandle_t fifo, mailbox;
void fail(const char *message) {
  printf("EROARE ENCODER: %s\n",message); Serial.flush(); for (;;) {}
}
void acquisitionTask(void *) {
  TickType_t wake=xTaskGetTickCount();
  uint32_t number=0, presses=0, dropped=0;
  int32_t previous=0;
  EncoderProcessor processor={};
  for (;;) {
    const EncoderReading reading=encoderRead();
    EncoderSignals s={};
    s.sampledAtMs=millis();
    buttonUpdate(button,s.sampledAtMs);
    if (buttonWasPressed(button)) ++presses;
    s.position=reading.position;
    s.invalidTransitions=reading.invalidTransitions;
    s.number=++number; s.presses=presses; s.pressed=buttonIsPressed(button);
    s.direction=s.position>previous ? 1 : s.position<previous ? -1 : 0;
    previous=s.position; s.dropped=dropped;
    if (filtering) {
      if (xQueueSendToBack(fifo,&s,0)!=pdTRUE) ++dropped;
    } else {
      encoderProcess(processor,s,false,cyclesPerRevolution);
      xQueueOverwrite(mailbox,&s);
    }
    vTaskDelayUntil(&wake,rtosNominalTicksForMs(acquisitionMs));
  }
}
void conditionTask(void *) {
  EncoderProcessor processor={};
  vTaskDelay(rtosNominalTicksForMs(10));
  TickType_t wake=xTaskGetTickCount();
  for (;;) {
    EncoderSignals s;
    while (xQueueReceive(fifo,&s,0)==pdTRUE) {
      encoderProcess(processor,s,true,cyclesPerRevolution);
      xQueueOverwrite(mailbox,&s);
    }
    vTaskDelayUntil(&wake,rtosNominalTicksForMs(acquisitionMs));
  }
}
void reportTask(void *) {
  vTaskDelay(rtosNominalTicksForMs(50));
  TickType_t wake=xTaskGetTickCount();
  for (;;) {
    EncoderSignals s;
    if (xQueuePeek(mailbox,&s,0)==pdTRUE) encoderPrintReport(s,millis());
    vTaskDelayUntil(&wake,rtosNominalTicksForMs(500));
  }
}
}
void encoderAppSetup(bool enableFiltering) {
  filtering=enableFiltering;
  serialStdioInit(115200);
  buttonInit(button,22,30,true);
  if (!encoderInit(2,3)) fail("pini intrerupere");
  printf("Lab %s encoder CLK=D2 DT=D3 SW=D22 | cicluri/tur=%u\n",
    filtering ? "3.2" : "3.1",(unsigned)cyclesPerRevolution);
  printf("Pozitie relativa la pornire. Conversie impulsuri -> grade; fara ADC.\n");
  mailbox=xQueueCreate(1,sizeof(EncoderSignals));
  if (!mailbox) fail("mailbox");
  if (filtering) {
    fifo=xQueueCreate(8,sizeof(EncoderSignals));
    if (!fifo) fail("FIFO");
    if (xTaskCreate(conditionTask,"EncFilter",512,NULL,2,NULL)!=pdPASS) fail("filtrare");
  }
  if (xTaskCreate(acquisitionTask,"EncRead",512,NULL,3,NULL)!=pdPASS) fail("achizitie");
  if (xTaskCreate(reportTask,"EncReport",768,NULL,1,NULL)!=pdPASS) fail("raport");
  Serial.flush();
}
