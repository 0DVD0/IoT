#include "app_lab_3.1_joystick.h"
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <stdio.h>
#include "dd_joystick.h"
#include "joystickSignals.h"
#include "joystickReport.h"
#include "ser_s_stdio.h"
#include "rtosTime.h"

namespace {
const uint16_t acquisitionPeriodMs = 20;
const uint16_t reportPeriodMs = 500;
const uint16_t reportOffsetMs = 50;
Joystick joystick;
const JoystickCalibration calibration;
QueueHandle_t joystickMailbox = NULL;

void taskAcquisition(void *) {
  JoystickSignals signals = {};
  TickType_t lastWakeTime = xTaskGetTickCount();

  for (;;) {
    joystickSignalsUpdate(signals, joystickRead(joystick), calibration);

    // Publica atomic ultima structura completa.
    xQueueOverwrite(joystickMailbox, &signals);

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
  vTaskDelay(rtosNominalTicksForMs(reportOffsetMs));

  TickType_t lastWakeTime = xTaskGetTickCount();

  for (;;) {
    JoystickSignals snapshot;

    if (xQueuePeek(joystickMailbox, &snapshot, 0) == pdTRUE) {
      joystickPrintReport(snapshot, millis());
    } else {
      printf("[JOYSTICK] Astept prima masuratoare...\n");
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

void lab31JoystickSetup() {
  serialStdioInit(115200);
  joystickInit(joystick, A0, A1, 22, 30);

  printf("Lab 3.1 - Joystick analogic KY023\n");
  printf("X=A0 | Y=A1 | Buton=D22\n");
  printf("X pozitiv: dreapta | Y pozitiv: sus\n");

  printf(
    "Tick=%u ms | achizitie=%u tick | raport=%u tick\n",
    (unsigned int)portTICK_PERIOD_MS,
    (unsigned int)rtosNominalTicksForMs(acquisitionPeriodMs),
    (unsigned int)rtosNominalTicksForMs(reportPeriodMs)
  );

  joystickMailbox =
    xQueueCreate(1, sizeof(JoystickSignals));

  if (joystickMailbox == NULL) {
    stopWithError("Creare mailbox esuata.");
  }

  if (xTaskCreate(
        taskAcquisition,
        "JoystickRead",
        256,
        NULL,
        2,
        NULL
      ) != pdPASS) {
    stopWithError("Creare task achizitie esuata.");
  }

  if (xTaskCreate(
        taskReport,
        "JoystickReport",
        512,
        NULL,
        1,
        NULL
      ) != pdPASS) {
    stopWithError("Creare task raportare esuata.");
  }

  Serial.flush();
}

void lab31JoystickLoop() {
  // Executia este gestionata de FreeRTOS.
}
