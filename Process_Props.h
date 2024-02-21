#include <unistd.h>
#include <termios.h>

#ifndef PROCESS_PROPS_H
#define PROCESS_PROPS_H

typedef struct {
    pid_t pid;
    int in_foreground;
    int is_suspended;
    int job_id;
    char * starting_command;
    struct termios process_termios;
} Process_Props;

Process_Props * newProcess_Props(pid_t pid, int in_foreground, int job_id, char * starting_command, struct termios process_termios);

// Getters
pid_t get_pid(Process_Props * input);
int get_in_foreground(Process_Props * input);
int get_is_suspended(Process_Props * input);
int get_job_id(Process_Props * input);
char * get_starting_command(Process_Props * input);
struct termios get_process_termios(Process_Props * input);

// Setters
void set_in_foreground(Process_Props * input, int to_set);
void set_is_suspended(Process_Props * input, int to_set);
void set_process_termios(Process_Props * input, struct termios to_set);



#endif
