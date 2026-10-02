#include "microphoneApp.h"
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include "dd_microphone.h"
#include "microphoneSignals.h"
#include "microphoneReport.h"
#include "ser_s_stdio.h"
#include "rtosTime.h"
namespace {
const uint8_t microphonePin = A0;
const uint16_t acquisitionPeriodMs = 100;
const uint16_t windowMs = 20;
const float relativeFullScaleMv = 500.0f;
bool conditioning;
QueueHandle_t inputQueue, outputMailbox;
void fail(const char *message) {
  printf("EROARE MICROFON: %s\n", message);
  Serial.flush();
  for (;;) {}
}
void acquisitionTask(void *) {
  TickType_t lastWake = xTaskGetTickCount();
  uint32_t number = 0, dropped = 0;
  MicrophoneProcessor processor = {};
  for (;;) {
    MicrophoneSignals s = {};
    s.reading = microphoneReadWindow(microphonePin, windowMs);
    s.number = ++number;
    s.dropped = dropped;
    if (conditioning) {
      if (xQueueSendToBack(inputQueue, &s, 0) != pdTRUE) ++dropped;
    } else {
      microphoneProcess(processor, s, false, relativeFullScaleMv);
      xQueueOverwrite(outputMailbox, &s);
    }
    vTaskDelayUntil(&lastWake, rtosNominalTicksForMs(acquisitionPeriodMs));
  }
}
void conditioningTask(void *) {
  MicrophoneProcessor processor = {};
  vTaskDelay(rtosNominalTicksForMs(30));
  TickType_t lastWake = xTaskGetTickCount();
  for (;;) {
    MicrophoneSignals s;
    while (xQueueReceive(inputQueue, &s, 0) == pdTRUE) {
      microphoneProcess(processor, s, true, relativeFullScaleMv);
      xQueueOverwrite(outputMailbox, &s);
    }
    vTaskDelayUntil(&lastWake, rtosNominalTicksForMs(acquisitionPeriodMs));
  }
}
void reportTask(void *) {
  vTaskDelay(rtosNominalTicksForMs(70));
  TickType_t lastWake = xTaskGetTickCount();
  for (;;) {
    MicrophoneSignals s;
    if (xQueuePeek(outputMailbox, &s, 0) == pdTRUE) microphonePrintReport(s, millis());
    vTaskDelayUntil(&lastWake, rtosNominalTicksForMs(500));
  }
}
}
void microphoneAppSetup(bool conditioningEnabled) {
  conditioning = conditioningEnabled;
  serialStdioInit(115200);
  microphoneInit(microphonePin);
  printf("Lab %s MICROFON AO=A0 | fereastra=20ms | referinta relativa=500mV P-P\n",
         conditioning ? "3.2" : "3.1");
  printf("Filtrare=%s | achizitie=%u tick | raport=%u tick | tick nominal=%u ms\n",
    conditioning ? "median5 + ponderi1,2,3" : "OPRITA",
    (unsigned)rtosNominalTicksForMs(100), (unsigned)rtosNominalTicksForMs(500),
    (unsigned)portTICK_PERIOD_MS);
  outputMailbox = xQueueCreate(1, sizeof(MicrophoneSignals));
  if (!outputMailbox) fail("mailbox");
  if (conditioning) {
    inputQueue = xQueueCreate(8, sizeof(MicrophoneSignals));
    if (!inputQueue) fail("FIFO");
    if (xTaskCreate(conditioningTask, "MicFilter", 512, NULL, 2, NULL) != pdPASS) fail("filtrare");
  }
  if (xTaskCreate(acquisitionTask, "MicRead", 512, NULL, 3, NULL) != pdPASS) fail("achizitie");
  if (xTaskCreate(reportTask, "MicReport", 640, NULL, 1, NULL) != pdPASS) fail("raportare");
  Serial.flush();
}
