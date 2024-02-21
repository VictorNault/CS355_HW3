#ifndef SIGHANDLERS_H
#define SIGHANDLERS_H



void sigchldhandler(int signal, siginfo_t *info, void *ucontext);

#endif