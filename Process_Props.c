#include <stdlib.h>

#include "common.h"
#include "Process_Props.h"

Process_Props * newProcess_Props(pid_t pid, int in_foreground, char * starting_command, struct termios process_termios) {
    Process_Props * new_process_props = malloc(sizeof(Process_Props));
    new_process_props->pid = pid;
    new_process_props->in_foreground = in_foreground;
    new_process_props->job_id = global_job_id_counter++;
    new_process_props->starting_command = starting_command;
    new_process_props->is_suspended = FALSE;
    new_process_props->process_termios;
}

// Getters
pid_t get_pid(Process_Props * input) {
    return input->pid;
}
int get_in_foreground(Process_Props * input) {
    return input->in_foreground;
}
int get_is_suspended(Process_Props * input) {
    return input->is_suspended;
}
int get_job_id(Process_Props * input) {
    return input->job_id;
}
char * get_starting_command(Process_Props * input) {
    return input->starting_command;
}
struct termios get_process_termios(Process_Props * input) {
    return input->process_termios;
}

// Setters
void set_in_foreground(Process_Props * input, int to_set) {
    input->in_foreground = to_set;
}
void set_is_suspended(Process_Props * input, int to_set) {
    input->is_suspended = to_set;
}
void set_process_termios(Process_Props * input, struct termios to_set) {
    input->process_termios = to_set;
}
