#include "app_lab_2.2.h"
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <stdio.h>
#include "dd_led.h"
#include "dd_button.h"
#include "ser_s_stdio.h"
#include "rtosByteBuffer.h"
#include "rtosTime.h"

namespace {
const uint8_t buttonPin = 22;
const uint8_t greenLedPin = 3;
const uint8_t redLedPin = 2;
const uint16_t buttonPeriodMs = 10;
const uint16_t maxN = 255; // 0 este delimitator, datele sunt octeti.
SemaphoreHandle_t buttonSemaphore = NULL;
RtosByteBuffer byteBuffer;
Button startButton;

void stopWithError(const char *message) {
  printf("EROARE: %s\n", message);
  Serial.flush();
  // Schedulerul porneste doar dupa revenirea din setup.
  for (;;) {}
}

void taskButtonLed(void *) {
  bool greenOn = false;
  uint32_t greenStartedMs = 0;
  TickType_t lastWake = xTaskGetTickCount();
  for (;;) {
    const uint32_t nowMs = millis();
    buttonUpdate(startButton, nowMs);
    if (buttonWasPressed(startButton)) {
      ledOn(greenLedPin);
      greenOn = true;
      greenStartedMs = nowMs;
      // Binar: cel mult o activare in asteptare, nu un contor de apasari.
      xSemaphoreGive(buttonSemaphore);
    }
    if (greenOn && static_cast<uint32_t>(nowMs - greenStartedMs) >= 1000) {
      ledOff(greenLedPin);
      greenOn = false;
    }
    xTaskDelayUntil(&lastWake, rtosTicksForMs(buttonPeriodMs));
  }
}

void taskSynchronous(void *) {
  uint16_t n = 0;
  for (;;) {
    if (xSemaphoreTake(buttonSemaphore, portMAX_DELAY) != pdTRUE) continue;
    if (n < maxN) ++n;
    for (uint16_t value = 1; value <= n; ++value) {
      byteBufferSendFront(byteBuffer, static_cast<uint8_t>(value));
      vTaskDelay(rtosTicksForMs(50));
    }
    // Un singur provider. Golirea impiedica delimitatorul introdus in fata
    // sa depaseasca datele mesajului. Consumerul ramane liber sa ruleze.
    while (byteBufferPending(byteBuffer)) vTaskDelay(1);
    byteBufferSendFront(byteBuffer, 0);
    while (byteBufferPending(byteBuffer)) vTaskDelay(1);
    for (uint16_t blink = 0; blink < n; ++blink) {
      ledOn(redLedPin);
      vTaskDelay(rtosTicksForMs(300));
      ledOff(redLedPin);
      vTaskDelay(rtosTicksForMs(500));
    }
  }
}

void taskAsynchronous(void *) {
  TickType_t lastWake = xTaskGetTickCount();
  bool newMessage = true;
  uint16_t receivedCount = 0;
  uint32_t messageNumber = 0;
  for (;;) {
    xTaskDelayUntil(&lastWake, rtosTicksForMs(200));
    uint8_t value;
    while (byteBufferReceive(byteBuffer, value)) {
      if (value == 0) {
        printf("| total=%u octeti\n", static_cast<unsigned>(receivedCount));
        receivedCount = 0;
        newMessage = true;
      } else {
        if (newMessage) {
          printf("[Mesaj %lu] RX: ", static_cast<unsigned long>(++messageNumber));
          newMessage = false;
        }
        printf("%u ", static_cast<unsigned>(value));
        ++receivedCount;
      }
    }
  }
}
}

void lab22Setup() {
  ledInit(greenLedPin);
  ledInit(redLedPin);
  buttonInit(startButton, buttonPin, 30, false); // Pull-down extern, apasat HIGH.
  serialStdioInit(115200);
  printf("Lab 2.2 FreeRTOS: D22 buton, D3 verde, D2 rosu\n");
  printf("Tick=%u Hz; T1=%u tick (10 ms ceruti, rotunjiti)\n",
         static_cast<unsigned>(configTICK_RATE_HZ),
         static_cast<unsigned>(rtosTicksForMs(buttonPeriodMs)));
  buttonSemaphore = xSemaphoreCreateBinary();
  if (!buttonSemaphore || !byteBufferInit(byteBuffer, 16))
    stopWithError("Alocare semafor/coada esuata");
  if (xTaskCreate(taskButtonLed, "ButtonLed", 192, NULL, 3, NULL) != pdPASS)
    stopWithError("Creare Task 1 esuata");
  if (xTaskCreate(taskSynchronous, "Sync", 256, NULL, 2, NULL) != pdPASS)
    stopWithError("Creare Task 2 esuata");
  if (xTaskCreate(taskAsynchronous, "Async", 384, NULL, 1, NULL) != pdPASS)
    stopWithError("Creare Task 3 esuata");
  Serial.flush();
}

void lab22Loop() {
  // Arduino_FreeRTOS porneste schedulerul dupa setup.
  // loop este apelat din Idle; nu blocam si nu raportam aici.
}
