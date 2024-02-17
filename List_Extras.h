#include <List.h>
#include <Process_Props.h>

#ifndef LIST_EXTRAS_H
#define LIST_EXTRAS_H

int delete_process(List *l, Process_Props * process);

Process_Props * get_by_pid(const List *l, pid_t pid);

#endif
