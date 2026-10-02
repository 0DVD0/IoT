#include "rtosByteBuffer.h"
bool byteBufferInit(RtosByteBuffer &buffer, UBaseType_t capacity) {
  if (buffer.queue != NULL || capacity == 0) return false;
  buffer.queue = xQueueCreate(capacity, sizeof(uint8_t));
  return buffer.queue != NULL;
}
bool byteBufferSendFront(RtosByteBuffer &buffer, uint8_t value, TickType_t timeout) {
  return buffer.queue && xQueueSendToFront(buffer.queue, &value, timeout) == pdTRUE;
}
bool byteBufferReceive(RtosByteBuffer &buffer, uint8_t &value, TickType_t timeout) {
  return buffer.queue && xQueueReceive(buffer.queue, &value, timeout) == pdTRUE;
}
UBaseType_t byteBufferPending(const RtosByteBuffer &buffer) {
  return buffer.queue ? uxQueueMessagesWaiting(buffer.queue) : 0;
}
