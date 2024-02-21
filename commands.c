#include "commands.h"

void bg(int pid){
    //check in the linked list to see if the process is suspended
    //int is_suspended = 
    // if(is_suspended){
    //     kill(pid,SIGCONT);
    //     //entering CR
    //      ***update the is_suspended flag in LL
    //      //exiting CR
    // }
    //else don't do anything because process is already running in background
}

void fg(int pid){
    //check in the linked list to see if the process is suspended
    //int is_suspended = 
    // if(is_suspended){
    //     kill(pid,SIGCONT);
    //     tcsetpgrp(1,pid); //set a process to run in foreground

    // }
    // else{
    //     tcsetpgrp(1,pid);
    // }
    //     //entering CR
    //      ***update the is_suspended flag in LL
    //      ***update the is_foreground flag in LL
    //      //exiting CR

    //waitpid(pid,NULL,0);
}

int myKill(int pid, int sig){
    //send a signal to pid
}

void printJobs(List * processes){
    struct node * temp = processes->head;
    int processCount = 1;
    print(processes);
    while (temp != NULL){
        Process_Props * nodeData = temp->data;
        // printf("%s", nodeData->starting_command);
        printf("[%d] %d %s\n",processCount, nodeData->is_suspended, nodeData->starting_command);
        processCount+=1; 
        temp = temp->next;
    }
}