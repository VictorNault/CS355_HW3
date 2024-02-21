

#ifndef PROCESS_PROPS_H
#define PROCESS_PROPS_H
#include <unistd.h>

typedef struct {
    pid_t pid;
    int in_foreground;
    int is_suspended;
    char * starting_command;
} Process_Props;

Process_Props * newProcess_Props(pid_t pid, int in_foreground, char * starting_command);

// Getters
pid_t get_pid(Process_Props * input);
int get_in_foreground(Process_Props * input);
int get_is_suspended(Process_Props * input);
char * get_starting_command(Process_Props * input);

// Setters
void set_in_foreground(Process_Props * input, int to_set);
void set_is_suspended(Process_Props * input, int to_set);



#endif
