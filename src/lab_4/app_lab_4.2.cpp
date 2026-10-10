#include "app_lab_4.2.h"
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <stdio.h>
#include "dcMotor.h"
#include "motorCommand.h"
#include "motorReport.h"
#include "rtosSerialStdio.h"
#include "rtosTime.h"
namespace {
const uint8_t enablePin=5, in1Pin=6, in2Pin=7;
const size_t lineCapacity=64;
DcMotor motor;
QueueHandle_t commands, stateMailbox;
TaskHandle_t reporter;
void fail(const char *message) {
  l298Coast(motor.driver);
  printf("EROARE INITIALIZARE: %s\n",message); Serial.flush(); for (;;) {}
}
void receiveTask(void *) {
  char line[lineCapacity]; size_t length=0; bool overflow=false;
  for (;;) {
    char c;
    if (scanf("%c",&c)!=1) { clearerr(stdin); vTaskDelay(1); continue; }
    if (c=='\n') {
      if (!length && !overflow) continue;
      line[length]=0;
      MotorCommand command=overflow ? MotorCommand{MotorAction::tooLong,0}
                                    : motorParseCommand(line);
      if (command.action==MotorAction::stop) {
        // Nu executa ulterior comenzile vechi ramase in asteptare.
        xQueueReset(commands);
        xQueueSendToFront(commands,&command,portMAX_DELAY);
      } else xQueueSendToBack(commands,&command,portMAX_DELAY);
      length=0; overflow=false;
    } else if (c=='\b' || c==127) {
      if (!overflow && length) --length;
    } else if ((unsigned char)c>=32 && !overflow) {
      if (length<lineCapacity-1) line[length++]=c;
      else overflow=true;
    }
  }
}
void controlTask(void *) {
  uint32_t number=0;
  const char *message="Pornire OFF";
  bool publish=true;
  for (;;) {
    MotorCommand command;
    if (xQueueReceive(commands,&command,1)==pdTRUE) {
      ++number; publish=true;
      const uint32_t now=millis();
      switch (command.action) {
        case MotorAction::set: motorSet(motor,command.percent,now); message="OK set"; break;
        case MotorAction::stop: motorStop(motor,now); message="OK stop - iesire dezactivata"; break;
        case MotorAction::maximum: motorMax(motor,now); message="OK max - limita 100%"; break;
        case MotorAction::increase: message=motorIncrease(motor,now) ? "OK inc - limita maxima" : "OK inc"; break;
        case MotorAction::decrease: message=motorDecrease(motor,now) ? "OK dec - limita zero" : "OK dec"; break;
        case MotorAction::status: message="Stare curenta"; break;
        case MotorAction::help: message="HELP"; break;
        case MotorAction::tooLong: message="EROARE linie prea lunga; comanda ignorata"; break;
        default: message="EROARE comanda sau valoare invalida; foloseste help"; break;
      }
    }
    const int16_t before=motor.appliedPercent;
    const bool wasWaiting=motor.waiting;
    motorUpdate(motor,millis());
    if (before!=motor.appliedPercent || wasWaiting!=motor.waiting) {
      publish=true; message="Iesire actualizata";
    }
    if (publish) {
      MotorSnapshot snapshot={motor.targetPercent,motor.appliedPercent,motor.direction,
        motorPwm(motor.appliedPercent),motor.waiting,number,message};
      xQueueOverwrite(stateMailbox,&snapshot);
      xTaskNotifyGive(reporter);
      publish=false;
    }
  }
}
void reportTask(void *) {
  uint32_t lastHelpNumber=0;
  for (;;) {
    ulTaskNotifyTake(pdTRUE,rtosTicksForMs(1000));
    MotorSnapshot snapshot;
    if (xQueuePeek(stateMailbox,&snapshot,0)==pdTRUE) {
      motorPrintReport(snapshot);
      if (snapshot.message[0]=='H' && snapshot.commandNumber!=lastHelpNumber) {
        motorPrintHelp(); lastHelpNumber=snapshot.commandNumber;
      }
    }
  }
}
}
void lab42Setup() {
  motorInit(motor,enablePin,in1Pin,in2Pin);
  rtosSerialStdioInit(115200);
  printf("Lab 4.2 L298 | ENA=D5 IN1=D6 IN2=D7 | Serial 115200\n");
  printf("Raportare duty-cycle comandat, nu turatie masurata. Inc/dec modifica modulul puterii.\n");
  motorPrintHelp();
  commands=xQueueCreate(8,sizeof(MotorCommand));
  stateMailbox=xQueueCreate(1,sizeof(MotorSnapshot));
  if (!commands || !stateMailbox) fail("cozi");
  if (xTaskCreate(reportTask,"MotorReport",640,NULL,1,&reporter)!=pdPASS) fail("raportare");
  if (xTaskCreate(controlTask,"MotorControl",384,NULL,3,NULL)!=pdPASS) fail("control");
  if (xTaskCreate(receiveTask,"MotorReceive",640,NULL,2,NULL)!=pdPASS) fail("receptie");
  Serial.flush();
}
void lab42Loop() {}
