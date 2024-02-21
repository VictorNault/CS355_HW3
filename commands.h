#ifndef COMMANDS_H
#define COMMANDS_H

#include "sighandlers.h"
#include "node.h"

void bg(int pid);
void fg(int pid);
int myKill(int pid, int isSIGKILL);

#endif