#include "commands.h"

//might be job id actually
void bg(int pid){
    
    //check in the linked list to see if the process is suspended

    // also don't think we need to update is_suspended flag here since child will signal
    pthread_mutex_lock(&mutex);
    Process_Props * process = get_by_pid(processes,pid);
    pthread_mutex_unlock(&mutex);

    if(!process){
        printf("%d: process not found\n",pid);
        return;
    }

    if(process->is_suspended){
        kill(process->pid,SIGCONT);
        set_is_suspended(process,FALSE);
    }

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
    pthread_mutex_lock(&mutex);
    Process_Props * process = get_by_pid(processes,pid);
    pthread_mutex_unlock(&mutex);

    if(!process){
        printf("%d: process not found\n",pid);
        return;
    }

    if (process->is_suspended == TRUE){
        kill(process->pid, SIGCONT);
        set_is_suspended(process,FALSE);
        tcsetpgrp(STDIN_FILENO, pid);
    }
    else{
        tcsetpgrp(STDIN_FILENO,pid);
    }


    //same here as comment above // process->is_suspended = FALSE;
    process->in_foreground = TRUE;
    // tcsetattr(STDIN_FILENO,TCSANOW, &process->process_termios);
    waitpid(process->pid,NULL,WUNTRACED);
    tcsetpgrp(STDIN_FILENO, shellPid);
    tcsetattr(STDIN_FILENO,TCSANOW, &shellTermios);
    

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
    int success = kill(pid,sig);
    if (success) return success;
    else{
        perror("kill:");
        return success;
    }
}

void printJobs(List * processes){
    struct node * temp = processes->head;
    int processCount = 1;
    print_processes(processes);
}