#ifndef SIGHANDLERS_H
#define SIGHANDLERS_H

#define _XOPEN_SOURCE 700
#include <regex.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/types.h>  
#include <sys/wait.h>
#include <signal.h>
#include "common.h"
#include <pthread.h>
#include "List.h"
#include "List_Extras.h"
#include "node.h"

void sigchldhandler(int signal, siginfo_t *info, void *ucontext);

#endif