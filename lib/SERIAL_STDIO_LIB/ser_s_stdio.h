#ifndef SER_S_STDIO_H
#define SER_S_STDIO_H

#include <stdio.h>

// Adaptor AVR (Uno/Mega): STDIO prin Serial, cu ecou si citire blocanta.
// Converteste CR, LF si CRLF de la terminal intr-un singur '\n'.
void serialStdioInit(unsigned long baud);

#endif
