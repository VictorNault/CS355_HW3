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


pthread_mutex_t mutex; //defining the mutex

char * delimiters = " \n";
int cmdLen  = 0;


void sigStopHandler(int sig){
    signal(SIGSTOP, sigStopHandler);
}

void sigIntHandler(int sig){
    signal(SIGINT, sigIntHandler);
}

void bg(int pid){
    //check in the linked list to see if the process is suspended
    //int is_suspended = 
    // if(is_suspended){
    //     kill(pid,SIGCONT);
    //     tcsetpgrp(NULL,pid); //set a process to be in background
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
    //waitpid(pid,NULL,0);
}

void sigchldhandler(int signal, siginfo_t *info, void *ucontext){
    if(info->si_code == CLD_EXITED || info->si_code == CLD_DUMPED){ //si_code in the info struct contains if the child is suspended, exited, or interrupted...
        //Entering critical region
        pthread_mutex_lock(&mutex);
        //***delete node in linkedlist
        pthread_mutex_unlock(&mutex);
    }
    else if(info->si_code == CLD_STOPPED){
        //Entering critical region
        pthread_mutex_lock(&mutex);
        //***update flag in node
        pthread_mutex_unlock(&mutex);
    }
    else if(info->si_code == CLD_CONTINUED){
        //Entering critical region
        pthread_mutex_lock(&mutex);
        //***update flag in node
        pthread_mutex_unlock(&mutex);
    }
}


char ** splitStringFromDelims(char * stringToSplit, int * cmdLen, int * background){
    *background = 0; 
    //counting the number of arguments passed by calling strtok twice (not the most efficient :()
    char * stringToSplitCopy = malloc( sizeof(char) * (strlen(stringToSplit)+1));
    strcpy(stringToSplitCopy, stringToSplit); 
    char * token = strtok(stringToSplitCopy,delimiters);
    *cmdLen = 0;
    while (token != NULL){
        (*cmdLen)++;
        token = strtok(NULL,delimiters);
    }

    if (*cmdLen == 0){ //nothing was passed
        return NULL;
    }

    char ** tokenList = (char **) malloc(sizeof(char *) * (*cmdLen));

    token = strtok(stringToSplit, delimiters);
    tokenList[0] = (char *) malloc(sizeof(char)* (strlen(token)+1));
    strcpy(tokenList[0],token);
    for (int i = 1; i < *cmdLen; i++){
        token = strtok(NULL, delimiters);
        tokenList[i] = (char *) malloc(sizeof(char)* (strlen(token)+1));
        strcpy(tokenList[i],token);
    }


    //checking if it should be backgrounded:
    if (strcmp(tokenList[(*cmdLen)-1],"&") == 0 && *cmdLen > 1 ){
        *background = TRUE;
        tokenList[(*cmdLen)-1] = NULL;
        // char ** temp = (char ** ) malloc(sizeof(char *) * ((*cmdLen)-1));
        // for(int i = 0; i < *cmdLen; i++){

        // }
    }

    else if(tokenList[(*cmdLen)-1][strlen(tokenList[(*cmdLen)-1])-1] == '&' && strlen(tokenList[(*cmdLen)-1]) > 1){
        tokenList[(*cmdLen)-1][strlen(tokenList[(*cmdLen)-1])-1] = '\0';

    }

    // for (int i = 0; i < count; i++){
    //     printf("%s testing", tokenList[i]);
    // }
    // printf("count: %d\n", cmdLen);
    free(stringToSplitCopy);
    return tokenList;
}

void emptySigHandler(){}

int getNthHistory(int n, char *** currentCommand, int top, int * cmdlen, int * background){
        HISTORY_STATE * history = history_get_history_state();
        HIST_ENTRY ** histlist = history_list();
        char * commandToCopy;

        if (n > history->length || n == 0){
            return 0; // failure
        }
        if (n == -1){
        commandToCopy = histlist[history->length-1]->line;
        }
        else if(top == 1){
            commandToCopy = histlist[n-1]->line;
        }
        else{
            commandToCopy = histlist[history->length-n]->line;
        }

        add_history(commandToCopy);
        
        char * commandCopy = malloc( sizeof(char) * (strlen(commandToCopy)+1)); // making a copy because of how readline handles history
        strcpy(commandCopy, commandToCopy);
        for (int i = 0; i < cmdLen; i++){
        free(*currentCommand[i]);
        }

        free(*currentCommand);
        printf("%s\n",commandCopy);
        *currentCommand = splitStringFromDelims(commandCopy, cmdlen, background);
        for (int i = 0; i < cmdLen; i++){
            // printf("\n%d: %s\n",i,currentCommand[i]);
        }
        free(commandCopy);
        free(history);
        return 1;
}

int main(){
    int status;
    signal(SIGINT, sigIntHandler);
    signal(SIGTSTP, sigStopHandler);
    sigset_t *sigset = (sigset_t*) malloc(sizeof(sigset_t));
    sigfillset(sigset);
    signal(SIGINT, sigIntHandler);
    signal(SIGTSTP, sigStopHandler);
    struct sigaction my_sigaction; //declaring the struct that contains the pointer to sighandler with extra information and flag
    my_sigaction.sa_sigaction = sigchldhandler; //setting the sighandler 
    my_sigaction.sa_flags = SA_SIGINFO; //setting the flag to say we want more information
    my_sigaction.sa_mask = *sigset;
    sigaction(SIGCHLD,&my_sigaction,NULL); //this is like signal(), setting a handler for SIGCHLD
    //ignoring all catchacble signals
    for(int i = 0; i <= SIGRTMAX; i++){
        signal(i,emptySigHandler);
    }

    //attempt to do get and set histsize environment variable.

    //long histSize;
    // char * histSizeStr = getenv("HISTSIZE");
    // if (histSize == NULL){
    //     histSize = 50;
    // }
    // else{
    //     histSize = atoi(histSizeStr);
    // }

    //setting up regex comparisions
    regex_t nregex;
    regex_t dashNRegex;


    int compare = regcomp(&nregex,"^![0-9]+$",REG_EXTENDED); //match numbers after ! \b is word boundry
    int dashCompare = regcomp(&dashNRegex,"^!-[0-9]+$",REG_EXTENDED);
    while(1){
        int addToHistory = TRUE;

        char * commandToParse = readline("prompt$ \n");  
        if (!commandToParse){
            // printf("ctrl-d, exiting...\n");
            exit(0);
        } 

        char * commandCopy = malloc( sizeof(char) * (strlen(commandToParse)+1)); // making a copy because of how readline handles history
        // beacuse strtok replaces with null byte
        strcpy(commandCopy, commandToParse);
        int commandLength; //split string from delim
        int background;
        char ** currentCommand = splitStringFromDelims(commandCopy, &commandLength, &background);
        free(commandCopy);
        
        if (currentCommand == NULL) continue;

        if (strcmp(currentCommand[0],"exit") == 0) {
            for (int i = 0; i < commandLength; i++){
                free(currentCommand[i]);
            }
            free(currentCommand);
            free(commandToParse);


            HISTORY_STATE * history = history_get_history_state();
            HIST_ENTRY ** histlist = history_list();

            for (int i = 0; i < history->length; i++){
                free_history_entry(histlist[i]);
            }
            free(history);
            regfree(&nregex);
            regfree(&dashNRegex);
            exit(0);
        }
        
        if(strcmp(currentCommand[0],"history") == 0){
            HISTORY_STATE * history = history_get_history_state();
            HIST_ENTRY ** histlist = history_list();

            for (int i = 0; i < history->length; i++){
                printf("%d  %s\n",i+1, histlist[i]->line);
                // free_history_entry(histlist[i]);
                
            }
             for (int i = 0; i < commandLength; i++){
            free(currentCommand[i]);
            }   

            free(history);
            free(currentCommand);
            free(commandToParse);;
            continue;   
        }
        if (commandLength == 1){
            if(strcmp(currentCommand[0], "!!") == 0){
                getNthHistory(-1,&currentCommand, 0, &commandLength, &background);
                addToHistory =0;
            }

            else if (regexec(&nregex,currentCommand[0],0,NULL,0) == 0){ // successful match returns 0
                addToHistory =0;
                char numberStr[strlen(currentCommand[0])];
                strcpy(numberStr, currentCommand[0]+1); // removing !
                int number = atoi(numberStr);
                int success = getNthHistory(number,&currentCommand, 1, &commandLength, &background);
                if (!success){ //e.g number too high 
 
                    printf("Please enter a valid history number\n");
                    for (int i = 0; i < commandLength; i++){
                            free(currentCommand[i]);
                    }
                        free(currentCommand);
                        free(commandToParse);
                        continue;
                }
            }

            else if (regexec(&dashNRegex,currentCommand[0],0,NULL,0) == 0){ // successful match returns 0
                addToHistory =0;
                char numberStr[strlen(currentCommand[0])];
                strcpy(numberStr, currentCommand[0]+2); // removing !-
                int number = atoi(numberStr);
                int success = getNthHistory(number,&currentCommand, 0, &commandLength, &background);
                if (!success){ //e.g number too high 
 
                    printf("Please enter a valid history number\n");
                    for (int i = 0; i < commandLength; i++){
                            free(currentCommand[i]);
                    }
                        free(currentCommand);
                        free(commandToParse);
                        continue;
                }
            }
        

        }

        pid_t parentPid = getpid();
        pid_t pid = fork();

        if (background == TRUE){
            tcsetpgrp(1, parentPid);
        }
        if (pid == 0){
            setpgid(getpid(),getpid());
            if (background == FALSE){
                tcsetpgrp(1,getpid());
            }
            char * args[commandLength+1];
            // printf("CommnadLen %d \n", *commandLength);
            for (int i = 0; i < commandLength; i++){
                args[i] = currentCommand[i];
                // printf("arg %d: %s \n", i,args[i]);
            }
            args[commandLength] = NULL;
            int success = execvp(currentCommand[0],args);
            perror("Error: ");
            exit(1);
        } else {
            if (background == FALSE){
            waitpid(pid,&status, 0);
            }
            if(addToHistory) add_history(commandToParse);
        }

        for (int i = 0; i < commandLength; i++){
            free(currentCommand[i]);
        }
        free(currentCommand);
        free(commandToParse);
    }
}

//