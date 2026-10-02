#include "app_lab_3.2_ntc.h"
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <stdio.h>
#include "dd_ntc.h"
#include "ntcConditioning.h"
#include "ntcConditionReport.h"
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
const float minimumTemperatureC = -20.0f;
const float maximumTemperatureC = 80.0f;
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
    NtcAdcSample sample = {};
    sample.number = ++number;
    sample.originalAdc = ntcReadAdc(sensorPin);
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
  NtcConditioner conditioner = {};
  vTaskDelay(rtosNominalTicksForMs(conditioningOffsetMs));
  TickType_t lastWake = xTaskGetTickCount();
  for (;;) {
    NtcAdcSample sample;
    while (xQueueReceive(acquisitionQueue, &sample, 0) == pdTRUE) {
      ntcConditionSample(conditioner, sample, minimumTemperatureC, maximumTemperatureC);
      xQueueOverwrite(resultMailbox, &conditioner.signals);
    }
    vTaskDelayUntil(&lastWake, rtosNominalTicksForMs(conditioningPeriodMs));
  }
}
void taskReport(void *) {
  vTaskDelay(rtosNominalTicksForMs(reportOffsetMs));
  TickType_t lastWake = xTaskGetTickCount();
  for (;;) {
    NtcProcessedSignals snapshot;
    if (xQueuePeek(resultMailbox, &snapshot, 0) == pdTRUE)
      ntcConditionPrintReport(snapshot, millis());
    else printf("[NTC 3.2] Astept date...\n");
    vTaskDelayUntil(&lastWake, rtosNominalTicksForMs(reportPeriodMs));
  }
}
}
void lab32NtcSetup() {
  serialStdioInit(115200);
  ntcInit(sensorPin);
  printf("Lab 3.2 NTC: median 5 -> ponderi 1,2,3; domeniu -20..80 C\n");
  printf("Zgomot artificial: %s | tick nominal=%u ms\n",
         injectTestNoise ? "ACTIV" : "OPRIT", (unsigned)portTICK_PERIOD_MS);
  acquisitionQueue = xQueueCreate(8, sizeof(NtcAdcSample));
  resultMailbox = xQueueCreate(1, sizeof(NtcProcessedSignals));
  if (!acquisitionQueue || !resultMailbox) stopWithError("Creare cozi esuata");
  if (xTaskCreate(taskAcquisition, "Acquisition", 256, NULL, 3, NULL) != pdPASS)
    stopWithError("Task achizitie");
  if (xTaskCreate(taskConditioning, "Conditioning", 512, NULL, 2, NULL) != pdPASS)
    stopWithError("Task conditionare");
  if (xTaskCreate(taskReport, "Report", 512, NULL, 1, NULL) != pdPASS)
    stopWithError("Task raportare");
  Serial.flush();
}
void lab32NtcLoop() { /* Schedulerul FreeRTOS gestioneaza task-urile. */ }
