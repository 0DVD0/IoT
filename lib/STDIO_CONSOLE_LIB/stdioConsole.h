#ifndef STDIO_CONSOLE_H
#define STDIO_CONSOLE_H

#include <stddef.h>

enum class ConsoleReadStatus {
  ok,
  lineTooLong,
  inputError,
  invalidArgument
};

// Citeste blocant din stdin pana la '\n', fara terminator in rezultat.
// capacity include locul pentru '\0'. O linie prea lunga este consumata
// complet si respinsa, astfel incat urmatoarea citire sa inceapa corect.
ConsoleReadStatus consoleReadLine(char *buffer, size_t capacity);

#endif
