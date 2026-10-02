#include "app_lab_3.1_ntc.h"
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <stdio.h>
#include "dd_ntc.h"
#include "temperatureSignals.h"
#include "temperatureReport.h"
#include "ser_s_stdio.h"
#include "rtosTime.h"

namespace {
const uint8_t sensorPin = A0;
const float sensorBeta = 3950.0f;
const uint16_t acquisitionPeriodMs = 100;
const uint16_t reportPeriodMs = 500;
const uint16_t reportOffsetMs = 50;
QueueHandle_t sensorMailbox = NULL;

void taskAcquisition(void *) {
  TemperatureSignals signals = {};

  TickType_t lastWakeTime = xTaskGetTickCount();

  for (;;) {
    uint16_t rawAdc;
    float temperatureC;

    const bool valid =
      ntcRead(sensorPin, rawAdc, temperatureC, sensorBeta);

    temperatureSignalsUpdate(
      signals,
      rawAdc,
      temperatureC,
      valid,
      millis()
    );

    // Overwrite este destinat cozilor cu un singur element.
    xQueueOverwrite(sensorMailbox, &signals);

    vTaskDelayUntil(
      &lastWakeTime,
      rtosNominalTicksForMs(acquisitionPeriodMs)
    );
  }
}

// ============================================================
// Task raportare: consumer
// Offset initial, apoi raportare periodica.
// ============================================================

void taskReport(void *) {
  vTaskDelay(rtosNominalTicksForMs(reportOffsetMs));

  TickType_t lastWakeTime = xTaskGetTickCount();

  for (;;) {
    TemperatureSignals snapshot;

    // Peek copiaza ultima stare fara sa goleasca mailbox-ul.
    if (xQueuePeek(sensorMailbox, &snapshot, 0) == pdTRUE) {
      temperaturePrintReport(snapshot, millis());
    } else {
      printf("[LAB 3.1] Astept prima masuratoare...\n");
    }

    vTaskDelayUntil(
      &lastWakeTime,
      rtosNominalTicksForMs(reportPeriodMs)
    );
  }
}

// ============================================================
// Initializare aplicatie
// ============================================================

void stopWithError(const char *message) {
  printf("EROARE INITIALIZARE: %s\n", message);
  Serial.flush();

  while (true) {
  }
}

}

void lab31NtcSetup() {
  serialStdioInit(115200);
  ntcInit(sensorPin);

  printf("Laborator 3.1 - Achizitie temperatura NTC\n");
  printf("Senzor: A0 | Beta: 3950 | STDIO Serial\n");

  printf(
    "Tick nominal=%u ms | achizitie=%u tick | raport=%u tick\n",
    (unsigned int)portTICK_PERIOD_MS,
    (unsigned int)rtosNominalTicksForMs(acquisitionPeriodMs),
    (unsigned int)rtosNominalTicksForMs(reportPeriodMs)
  );

  sensorMailbox = xQueueCreate(1, sizeof(TemperatureSignals));

  if (sensorMailbox == NULL) {
    stopWithError("Nu se poate crea mailbox-ul.");
  }

  if (xTaskCreate(
        taskAcquisition,
        "Acquisition",
        384,
        NULL,
        2,
        NULL
      ) != pdPASS) {
    stopWithError("Nu se poate crea task-ul de achizitie.");
  }

  if (xTaskCreate(
        taskReport,
        "Report",
        512,
        NULL,
        1,
        NULL
      ) != pdPASS) {
    stopWithError("Nu se poate crea task-ul de raportare.");
  }

  Serial.flush();
}

void lab31NtcLoop() {
  // Schedulerul Arduino_FreeRTOS porneste dupa setup().
}
