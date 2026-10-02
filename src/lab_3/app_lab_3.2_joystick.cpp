#include "app_lab_3.2_joystick.h"
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <stdio.h>
#include "dd_joystick.h"
#include "joystickConditioning.h"
#include "joystickConditionReport.h"
#include "ser_s_stdio.h"
#include "rtosTime.h"
namespace {
Joystick joystick;
const JoystickConditionConfig calibration;
const uint16_t acquisitionPeriodMs = 20;
const uint16_t conditioningPeriodMs = 20;
const uint16_t conditioningOffsetMs = 10;
const uint16_t reportPeriodMs = 500;
const uint16_t reportOffsetMs = 50;
const bool injectTestNoise = false;
QueueHandle_t acquisitionQueue = NULL;
QueueHandle_t resultMailbox = NULL;
void stopWithError(const char *message) {
  printf("EROARE: %s\n", message);
  Serial.flush();
  for (;;) {}
}
void taskAcquisition(void *) {
  TickType_t lastWake = xTaskGetTickCount();
  uint32_t number = 0, dropped = 0, presses = 0;
  for (;;) {
    const JoystickReading reading = joystickRead(joystick);
    if (reading.pressEvent) ++presses;
    JoystickAdcSample sample = {};
    sample.number = ++number;
    sample.originalX = sample.inputX = reading.rawX;
    sample.originalY = sample.inputY = reading.rawY;
    sample.sampledAtMs = reading.sampledAtMs;
    sample.buttonPressed = reading.buttonPressed;
    sample.pressCount = presses;
    if (injectTestNoise && number % 40UL == 0) {
      sample.injected = true;
      const bool highPulse = (number / 40UL) % 2UL == 0;
      sample.inputX = highPulse ? 1023 : 0;
      sample.inputY = highPulse ? 0 : 1023;
    }
    sample.droppedCount = dropped;
    if (xQueueSendToBack(acquisitionQueue, &sample, 0) != pdTRUE) ++dropped;
    vTaskDelayUntil(&lastWake, rtosNominalTicksForMs(acquisitionPeriodMs));
  }
}
void taskConditioning(void *) {
  JoystickConditioner conditioner = {};
  vTaskDelay(rtosNominalTicksForMs(conditioningOffsetMs));
  TickType_t lastWake = xTaskGetTickCount();
  for (;;) {
    JoystickAdcSample sample;
    while (xQueueReceive(acquisitionQueue, &sample, 0) == pdTRUE) {
      joystickConditionSample(conditioner, sample, calibration);
      xQueueOverwrite(resultMailbox, &conditioner.signals);
    }
    vTaskDelayUntil(&lastWake, rtosNominalTicksForMs(conditioningPeriodMs));
  }
}
void taskReport(void *) {
  vTaskDelay(rtosNominalTicksForMs(reportOffsetMs));
  TickType_t lastWake = xTaskGetTickCount();
  for (;;) {
    JoystickProcessedSignals snapshot;
    if (xQueuePeek(resultMailbox, &snapshot, 0) == pdTRUE)
      joystickConditionPrintReport(snapshot, millis());
    else printf("[JOYSTICK 3.2] Astept date...\n");
    vTaskDelayUntil(&lastWake, rtosNominalTicksForMs(reportPeriodMs));
  }
}
}
void lab32JoystickSetup() {
  serialStdioInit(115200);
  joystickInit(joystick, A0, A1, 22, 30);
  printf("Lab 3.2 JOYSTICK: median 5 -> ponderi 1,2,3; X=A0 Y=A1 SEL=D22; iesire -100..100%%\n");
  printf("Zgomot artificial: %s | tick nominal=%u ms\n",
         injectTestNoise ? "ACTIV" : "OPRIT", (unsigned)portTICK_PERIOD_MS);
  acquisitionQueue = xQueueCreate(8, sizeof(JoystickAdcSample));
  resultMailbox = xQueueCreate(1, sizeof(JoystickProcessedSignals));
  if (!acquisitionQueue || !resultMailbox) stopWithError("Creare cozi esuata");
  if (xTaskCreate(taskAcquisition, "Acquisition", 320, NULL, 3, NULL) != pdPASS)
    stopWithError("Task achizitie");
  if (xTaskCreate(taskConditioning, "Conditioning", 640, NULL, 2, NULL) != pdPASS)
    stopWithError("Task conditionare");
  if (xTaskCreate(taskReport, "Report", 512, NULL, 1, NULL) != pdPASS)
    stopWithError("Task raportare");
  Serial.flush();
}
void lab32JoystickLoop() { /* Schedulerul FreeRTOS gestioneaza task-urile. */ }
