#include "app_lab_6.2.h"
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <queue.h>
#include <stdio.h>
#include <dd_button.h>
#include <trafficLight.h>
#include <trafficFsm.h>
#include <rtosSerialStdio.h>
namespace {
const TrafficLight eastLight = {4, 5, 6};
const TrafficLight northLight = {7, 8, 9};
const uint8_t northButtonPin = 22, pedestrianButtonPin = 24;
SemaphoreHandle_t northSignal, pedestrianSignal;
QueueHandle_t reports;
struct Snapshot { uint32_t tick; TrafficPhase phase; bool north, pedestrian; };
TickType_t ticksFor(uint32_t ms) {
  return static_cast<TickType_t>((ms + portTICK_PERIOD_MS - 1) / portTICK_PERIOD_MS);
}
void allRed() {
  trafficLightWrite(eastLight, TrafficColor::Red);
  trafficLightWrite(northLight, TrafficColor::Red);
}
void fail() {
  allRed(); printf("EROARE INIT: sistem mentinut pe ROSU\n"); Serial.flush();
  for (;;) {}
}
void requestTask(void *) {
  Button northButton, pedestrianButton;
  buttonInit(northButton, northButtonPin, 30, true);
  buttonInit(pedestrianButton, pedestrianButtonPin, 30, true);
  TickType_t wake = xTaskGetTickCount();
  for (;;) {
    const uint32_t now = millis();
    buttonUpdate(northButton, now); buttonUpdate(pedestrianButton, now);
    if (buttonWasPressed(northButton)) xSemaphoreGive(northSignal);
    if (buttonWasPressed(pedestrianButton)) xSemaphoreGive(pedestrianSignal);
    vTaskDelayUntil(&wake, ticksFor(10));
  }
}
void controlTask(void *) {
  TrafficFsm fsm({ticksFor(5000), ticksFor(5000), ticksFor(2000), ticksFor(1000), ticksFor(5000)});
  uint32_t now = 0;
  TickType_t previous = xTaskGetTickCount(), wake = previous;
  fsm.reset(now);
  Snapshot first = {0, fsm.phase(), false, false};
  xQueueOverwrite(reports, &first);
  for (;;) {
    const TickType_t tick = xTaskGetTickCount();
    now += static_cast<TickType_t>(tick - previous); // Extend AVR tick rollover.
    previous = tick;
    const TrafficPhase old = fsm.phase();
    const bool oldNorth = fsm.northPending(), oldPedestrian = fsm.pedestrianPending();
    const bool n = xSemaphoreTake(northSignal, 0) == pdTRUE;
    const bool p = xSemaphoreTake(pedestrianSignal, 0) == pdTRUE;
    fsm.update(now, n, p);
    if (old != fsm.phase()) {
      // Break before make: withdraw every permission before granting another.
      allRed();
      trafficLightWrite(eastLight, fsm.east());
      trafficLightWrite(northLight, fsm.north());
    }
    if (old != fsm.phase() || n || p || oldNorth != fsm.northPending() ||
        oldPedestrian != fsm.pedestrianPending()) {
      Snapshot s = {now, fsm.phase(), fsm.northPending(), fsm.pedestrianPending()};
      // Latest-state mailbox: slow Serial never blocks traffic control.
      xQueueOverwrite(reports, &s);
    }
    vTaskDelayUntil(&wake, ticksFor(10));
  }
}
void reportTask(void *) {
  Snapshot s;
  for (;;) {
    if (xQueueReceive(reports, &s, portMAX_DELAY) == pdTRUE)
      printf("[FSM] tick=%lu | %s | cerere Nord=%u | pietoni=%u\n",
             static_cast<unsigned long>(s.tick), TrafficFsm::name(s.phase),
             static_cast<unsigned>(s.north), static_cast<unsigned>(s.pedestrian));
  }
}
}
void lab62Setup() {
  trafficLightInit(eastLight); trafficLightInit(northLight);
  rtosSerialStdioInit(115200);
  printf("Lab 6.2 | Est R/Galben/V=4/5/6 | Nord R/Galben/V=7/8/9\n");
  printf("Butoane la GND: Nord=D22, pietoni=D24 | debounce 30 ms\n");
  printf("Est minim/Nord verde=5 s | galben=2 s | rosu comun=1 s | pietoni=5 s\n");
  printf("Tick nominal=%lu ms; duratele sunt rotunjite in sus la tick.\n",
         static_cast<unsigned long>(portTICK_PERIOD_MS));
  northSignal = xSemaphoreCreateBinary(); pedestrianSignal = xSemaphoreCreateBinary();
  reports = xQueueCreate(1, sizeof(Snapshot));
  if (!northSignal || !pedestrianSignal || !reports) fail();
  if (xTaskCreate(reportTask, "TrafficUI", 640, NULL, 1, NULL) != pdPASS ||
      xTaskCreate(requestTask, "Requests", 256, NULL, 2, NULL) != pdPASS ||
      xTaskCreate(controlTask, "TrafficFSM", 384, NULL, 3, NULL) != pdPASS) fail();
  Serial.flush();
}
void lab62Loop() {}
