#include "app_lab_4.1.h"
#include "dd_relay.h"
#include "ser_s_stdio.h"
#include <stdio.h>
#include <string.h>
namespace {
const uint8_t relayPin = 7;
const bool relayActiveLow = true;
const size_t commandCapacity = 40;
Relay actuator;
char commandBuffer[commandCapacity];
size_t commandLength = 0;
bool commandTooLong = false;
unsigned long invalidCommands = 0;
void printState() {
  printf("Stare comandata releu: %s\n", relayIsOn(actuator) ? "ON" : "OFF");
}
void printPrompt() { printf("cmd> "); }
void executeCommand(char *command) {
  while (*command == ' ' || *command == '\t') ++command;
  size_t length = strlen(command);
  while (length && (command[length-1] == ' ' || command[length-1] == '\t'))
    command[--length] = '\0';
  if (!length) return;
  if (strcmp(command, "relay on") == 0) {
    relayWrite(actuator, true);
    printf("OK: comanda relay on executata.\n");
    printState();
  } else if (strcmp(command, "relay off") == 0) {
    relayWrite(actuator, false);
    printf("OK: comanda relay off executata.\n");
    printState();
  } else if (strcmp(command, "relay status") == 0) {
    printState();
  } else if (strcmp(command, "help") == 0) {
    printf("Comenzi: relay on, relay off, relay status, help\n");
  } else {
    ++invalidCommands;
    printf("EROARE: comanda necunoscuta \"%s\". Erori=%lu\n", command, invalidCommands);
    printState();
    printf("Scrie help pentru lista comenzilor.\n");
  }
}
void finishCommand() {
  if (commandTooLong) {
    ++invalidCommands;
    printf("EROARE: comanda prea lunga, maximum %u caractere. Erori=%lu\n",
           static_cast<unsigned>(commandCapacity-1), invalidCommands);
    printState();
  } else {
    commandBuffer[commandLength]='\0';
    executeCommand(commandBuffer);
  }
  commandLength=0;
  commandTooLong=false;
  printPrompt();
}
}
void lab41Setup() {
  relayInit(actuator, relayPin, relayActiveLow);
  serialStdioInit(115200);
  commandLength=0; commandTooLong=false; invalidCommands=0;
  printf("\nLab 4.1 - Control releu prin STDIO\n");
  printf("Mega 2560 | IN=D7 | Serial=115200 baud\n");
  printf("Comenzi: relay on, relay off, relay status, help\n");
  printState(); printPrompt();
}
void lab41Loop() {
  char c;
  if (scanf("%c", &c) != 1) { clearerr(stdin); return; }
  // Adaptorul existent face ecoul si normalizeaza CR/LF/CRLF la un singur LF.
  if (c == '\n') { finishCommand(); return; }
  if (c == '\b' || c == 127) {
    if (!commandTooLong && commandLength) --commandLength;
    return;
  }
  if (static_cast<unsigned char>(c)<32 || commandTooLong) return;
  if (commandLength<commandCapacity-1) commandBuffer[commandLength++]=c;
  else commandTooLong=true;
}
