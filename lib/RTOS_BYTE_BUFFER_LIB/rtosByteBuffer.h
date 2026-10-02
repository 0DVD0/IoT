#ifndef RTOS_BYTE_BUFFER_H
#define RTOS_BYTE_BUFFER_H
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <stdint.h>

// Coada de octeti, detinuta de aplicatie si partajata intre task-uri.
// SendToFront implica ordine LIFO pentru datele acumulate.
struct RtosByteBuffer { QueueHandle_t queue = NULL; };
bool byteBufferInit(RtosByteBuffer &buffer, UBaseType_t capacity);
bool byteBufferSendFront(RtosByteBuffer &buffer, uint8_t value,
                         TickType_t timeout = portMAX_DELAY);
bool byteBufferReceive(RtosByteBuffer &buffer, uint8_t &value,
                       TickType_t timeout = 0);
UBaseType_t byteBufferPending(const RtosByteBuffer &buffer);
#endif
