#include "ser_s_stdio.h"
#include <Arduino.h>

static FILE serialStdOut;
static FILE serialStdIn;
static bool skipNextLf = false;

static int serialPutChar(char c, FILE *) {
  if (c == '\n') {
    Serial.write('\r');
  }
  Serial.write(c);
  return 0;
}

static int serialGetChar(FILE *) {
  while (true) {
    while (!Serial.available()) {
      // Asteapta un caracter de la terminal.
    }
    const int c = Serial.read();

    // CRLF reprezinta un singur Enter, chiar daca soseste in doua etape.
    if (skipNextLf && c == '\n') {
      skipNextLf = false;
      continue;
    }
    skipNextLf = (c == '\r');

    if (c == '\r' || c == '\n') {
      serialPutChar('\n', NULL);
      return '\n';
    }

    Serial.write(static_cast<uint8_t>(c));
    return c;
  }
}

void serialStdioInit(unsigned long baud) {
  Serial.begin(baud);
  skipNextLf = false;

  fdev_setup_stream(&serialStdOut, serialPutChar, NULL, _FDEV_SETUP_WRITE);
  fdev_setup_stream(&serialStdIn, NULL, serialGetChar, _FDEV_SETUP_READ);

  stdout = &serialStdOut;
  stdin = &serialStdIn;
}
