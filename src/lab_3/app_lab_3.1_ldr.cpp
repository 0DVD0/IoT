#include "app_lab_3.1_ldr.h"
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <stdio.h>
#include "dd_ldr.h"
#include "lightSignals.h"
#include "lightReport.h"
#include "ser_s_stdio.h"
#include "rtosTime.h"

namespace {
const uint8_t lightSensorPin = A0;
const uint16_t acquisitionPeriodMs = 100;
const uint16_t reportPeriodMs = 500;
const uint16_t reportOffsetMs = 50;
const LdrConfig ldrConfig;
QueueHandle_t lightMailbox = NULL;

void taskAcquisition(void *) {
  LightSignals signals = {};
  TickType_t lastWakeTime = xTaskGetTickCount();

  for (;;) {
    uint16_t rawAdc;
    float lux;

    const bool valid = ldrRead(lightSensorPin, rawAdc, lux, ldrConfig);

    lightSignalsUpdate(
      signals,
      rawAdc,
      lux,
      valid,
      millis()
    );

    // Coada are exact un element: ultima stare disponibila.
    xQueueOverwrite(lightMailbox, &signals);

    vTaskDelayUntil(
      &lastWakeTime,
      rtosNominalTicksForMs(acquisitionPeriodMs)
    );
  }
}

// ============================================================
// Task raportare - consumer
// ============================================================

void taskReport(void *) {
  // Offset initial fata de achizitie.
  vTaskDelay(rtosNominalTicksForMs(reportOffsetMs));

  TickType_t lastWakeTime = xTaskGetTickCount();

  for (;;) {
    LightSignals snapshot;

    // Copiere sincronizata a structurii complete.
    if (xQueuePeek(lightMailbox, &snapshot, 0) == pdTRUE) {
      lightPrintReport(snapshot, millis());
    } else {
      printf("[LDR] Astept prima masuratoare...\n");
    }

    vTaskDelayUntil(
      &lastWakeTime,
      rtosNominalTicksForMs(reportPeriodMs)
    );
  }
}

// ============================================================
// Setup / loop
// ============================================================

void stopWithError(const char *message) {
  printf("EROARE INITIALIZARE: %s\n", message);
  Serial.flush();

  while (true) {
  }
}

}

void lab31LdrSetup() {
  serialStdioInit(115200);
  ldrInit(lightSensorPin);

  printf("Lab 3.1 - Fotoresistor LDR pe A0\n");
  printf("FreeRTOS + STDIO | Serial 115200 baud\n");

  printf(
    "Tick=%u ms | achizitie=%u tick | raport=%u tick\n",
    (unsigned int)portTICK_PERIOD_MS,
    (unsigned int)rtosNominalTicksForMs(acquisitionPeriodMs),
    (unsigned int)rtosNominalTicksForMs(reportPeriodMs)
  );

  lightMailbox = xQueueCreate(1, sizeof(LightSignals));

  if (lightMailbox == NULL) {
    stopWithError("Creare mailbox esuata.");
  }

  if (xTaskCreate(
        taskAcquisition,
        "LightRead",
        384,
        NULL,
        2,
        NULL
      ) != pdPASS) {
    stopWithError("Creare task achizitie esuata.");
  }

  if (xTaskCreate(
        taskReport,
        "LightReport",
        512,
        NULL,
        1,
        NULL
      ) != pdPASS) {
    stopWithError("Creare task raportare esuata.");
  }

  Serial.flush();
}

void lab31LdrLoop() {
  // Arduino_FreeRTOS gestioneaza executia task-urilor.
}
