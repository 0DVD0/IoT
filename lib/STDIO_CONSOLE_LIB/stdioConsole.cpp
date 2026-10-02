#include "stdioConsole.h"
#include <stdio.h>

ConsoleReadStatus consoleReadLine(char *buffer, size_t capacity) {
  if (buffer == NULL || capacity == 0) {
    return ConsoleReadStatus::invalidArgument;
  }

  size_t length = 0;
  bool overflow = false;
  buffer[0] = '\0';

  while (true) {
    const int c = getchar();

    if (c == EOF) {
      buffer[0] = '\0';
      return ConsoleReadStatus::inputError;
    }

    if (c == '\n') {
      if (overflow) {
        buffer[0] = '\0';
        return ConsoleReadStatus::lineTooLong;
      }
      buffer[length] = '\0';
      return ConsoleReadStatus::ok;
    }

    if (length < capacity - 1) {
      buffer[length++] = static_cast<char>(c);
    } else {
      overflow = true;
    }
  }
}
