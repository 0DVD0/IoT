#include "motorCommand.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
MotorCommand motorParseCommand(const char *line) {
  MotorCommand result={MotorAction::invalid,0};
  char buffer[64];
  if (!line || strlen(line)>=sizeof(buffer)) return result;
  strcpy(buffer,line);
  char *p=buffer;
  while (isspace((unsigned char)*p)) ++p;
  size_t length=strlen(p);
  while (length && isspace((unsigned char)p[length-1])) p[--length]=0;
  if (!strcmp(p,"motor stop")) result.action=MotorAction::stop;
  else if (!strcmp(p,"motor max")) result.action=MotorAction::maximum;
  else if (!strcmp(p,"motor inc")) result.action=MotorAction::increase;
  else if (!strcmp(p,"motor dec")) result.action=MotorAction::decrease;
  else if (!strcmp(p,"motor status")) result.action=MotorAction::status;
  else if (!strcmp(p,"help")) result.action=MotorAction::help;
  else if (!strncmp(p,"motor set ",10)) {
    char *end;
    errno=0;
    long value=strtol(p+10,&end,10);
    if (end!=p+10 && *end==0 && !errno && value>=-100 && value<=100) {
      result.action=MotorAction::set; result.percent=(int16_t)value;
    }
  }
  return result;
}
