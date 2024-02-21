#define _XOPEN_SOURCE 700
#include "common.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <string.h>
#include <pthread.h>

//including the linked list data structure. The linked list contains nodes that have fields including the pid, is_foreground, 
//is_suspended, starting command, and respective methods. 
#include <List.h>

pid_t pid;

//define a global string delimeter
//define a global linked list to store processes

int bg(int pid){
    //make a process go to the background
}

int fg(int pid){
    //make a process go to the foreground
}

void jobs(){
    //print processes, oldest in the front, youngest in the back
}

void kill(int pid, int sig){
    //send a SIGTERM or SIGKILL to a process
}

void sigtstp(int sig){
    //send the process running in the foreground into the background
}

void sighandler(int sig){
    //don't do anything if the user press Ctrl+C 
}

//in the sigchldhandler, we will use mutexes to protect the critical region, which is
//whenever the linkedlist containing process info is modified
void sigchldhandler(siginfo_t *siginfo, void *context){
    //if child is exited, or dumped
        //Entering CR
        //delete the node from the linked list
        //Exiting CR

    //if child is stopped (suspended), update the flag
        //Entering CR
        //update the flag
        //Exiting CR
    
    //if child is resumed (continued), update the flag
        //Entering CR
        //update the flag
        //Exiting CR
}

int main(){
    //define sigaction to mask all signals (sa_mask)

    //set up a mask that ignores all signals, except for SIGCHLD

    //define a linked list buffer for potential multiple commands in case of semicolon

    while(TRUE){
        //define a flag for foreground or background

        //if there isn't a command in the buffer linked list

            //get the command string using readline

            //if the command string is null (Ctrl+D), exit the shell

            //parse the command string, store tokens in an array


        //fork off a child of a unique process group

        //if background, use tcsetpgrp to make the child not read from terminal

        //child 
        if(pid == 0){
            //wait for SIGUSR1
            //exec
            //if fails, exit
        }
        //parent
        else{
            //add the child to linked list, send SIGUSR1 to child to keep going

            //wait for child only if child is in foreground
        }
    }
}

