#include "app_lab_1.1.h"
#include "dd_led.h"
#include "ser_s_stdio.h"
#include "stdioConsole.h"

#include <stdio.h>
#include <string.h>

namespace {
const uint8_t ledPin = 7;
const unsigned long serialBaud = 9600;
const size_t commandCapacity = 32;

void executeCommand(const char *command) {
  if (strcmp(command, "led on") == 0) {
    ledOn(ledPin);
    printf("OK: LED aprins\n");
  } else if (strcmp(command, "led off") == 0) {
    ledOff(ledPin);
    printf("OK: LED stins\n");
  } else {
    printf("Eroare: comanda necunoscuta. Foloseste led on sau led off.\n");
  }
}
}

void lab11Setup() {
  ledInit(ledPin);
  serialStdioInit(serialBaud);

  printf("Lab 1.1 - Control LED prin STDIO\n");
  printf("Comenzi disponibile: led on, led off\n");
}

void lab11Loop() {
  char command[commandCapacity];

  printf("> ");
  const ConsoleReadStatus status = consoleReadLine(command, sizeof(command));

  if (status == ConsoleReadStatus::lineTooLong) {
    printf("Eroare: comanda prea lunga (maximum %u caractere).\n",
           static_cast<unsigned int>(sizeof(command) - 1));
    return;
  }

  if (status != ConsoleReadStatus::ok) {
    printf("Eroare: citirea comenzii a esuat.\n");
    return;
  }

  // Enter fara text nu modifica LED-ul.
  if (command[0] != '\0') {
    executeCommand(command);
  }
}
