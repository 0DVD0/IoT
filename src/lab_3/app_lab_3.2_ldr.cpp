#include "app_lab_3.2_ldr.h"
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <stdio.h>
#include "dd_ldr.h"
#include "ldrConditioning.h"
#include "ldrConditionReport.h"
#include "ser_s_stdio.h"
#include "rtosTime.h"
namespace {
const uint8_t sensorPin = A0;
const uint16_t acquisitionPeriodMs = 100;
const uint16_t conditioningPeriodMs = 100;
const uint16_t conditioningOffsetMs = 20;
const uint16_t reportPeriodMs = 500;
const uint16_t reportOffsetMs = 50;
const bool injectTestNoise = false;
const float minimumLux = 0.0f;
const float maximumLux = 1000.0f;
QueueHandle_t acquisitionQueue = NULL;
QueueHandle_t resultMailbox = NULL;
void stopWithError(const char *message) {
  printf("EROARE: %s\n", message);
  Serial.flush();
  for (;;) {}
}
void taskAcquisition(void *) {
  TickType_t lastWake = xTaskGetTickCount();
  uint32_t number = 0, dropped = 0;
  for (;;) {
    LdrAdcSample sample = {};
    sample.number = ++number;
    sample.originalAdc = ldrReadAdc(sensorPin);
    sample.inputAdc = sample.originalAdc;
    sample.sampledAtMs = millis();
    if (injectTestNoise && number % 20UL == 0) {
      sample.injected = true;
      sample.inputAdc = ((number / 20UL) % 2UL == 0) ? 1023 : 0;
    }
    sample.droppedCount = dropped;
    if (xQueueSendToBack(acquisitionQueue, &sample, 0) != pdTRUE) ++dropped;
    vTaskDelayUntil(&lastWake, rtosNominalTicksForMs(acquisitionPeriodMs));
  }
}
void taskConditioning(void *) {
  LdrConditioner conditioner = {};
  vTaskDelay(rtosNominalTicksForMs(conditioningOffsetMs));
  TickType_t lastWake = xTaskGetTickCount();
  for (;;) {
    LdrAdcSample sample;
    while (xQueueReceive(acquisitionQueue, &sample, 0) == pdTRUE) {
      ldrConditionSample(conditioner, sample, minimumLux, maximumLux);
      xQueueOverwrite(resultMailbox, &conditioner.signals);
    }
    vTaskDelayUntil(&lastWake, rtosNominalTicksForMs(conditioningPeriodMs));
  }
}
void taskReport(void *) {
  vTaskDelay(rtosNominalTicksForMs(reportOffsetMs));
  TickType_t lastWake = xTaskGetTickCount();
  for (;;) {
    LdrProcessedSignals snapshot;
    if (xQueuePeek(resultMailbox, &snapshot, 0) == pdTRUE)
      ldrConditionPrintReport(snapshot, millis());
    else printf("[LDR 3.2] Astept date...\n");
    vTaskDelayUntil(&lastWake, rtosNominalTicksForMs(reportPeriodMs));
  }
}
}
void lab32LdrSetup() {
  serialStdioInit(115200);
  ldrInit(sensorPin);
  printf("Lab 3.2 LDR: median 5 -> ponderi 1,2,3; domeniu 0..1000 lux estimati\n");
  printf("Zgomot artificial: %s | tick nominal=%u ms\n",
         injectTestNoise ? "ACTIV" : "OPRIT", (unsigned)portTICK_PERIOD_MS);
  acquisitionQueue = xQueueCreate(8, sizeof(LdrAdcSample));
  resultMailbox = xQueueCreate(1, sizeof(LdrProcessedSignals));
  if (!acquisitionQueue || !resultMailbox) stopWithError("Creare cozi esuata");
  if (xTaskCreate(taskAcquisition, "Acquisition", 256, NULL, 3, NULL) != pdPASS)
    stopWithError("Task achizitie");
  if (xTaskCreate(taskConditioning, "Conditioning", 512, NULL, 2, NULL) != pdPASS)
    stopWithError("Task conditionare");
  if (xTaskCreate(taskReport, "Report", 512, NULL, 1, NULL) != pdPASS)
    stopWithError("Task raportare");
  Serial.flush();
}
void lab32LdrLoop() { /* Schedulerul FreeRTOS gestioneaza task-urile. */ }
