#include "common.h"

char * delimiters = " \n";
int cmdLen  = 0;
List * processes;

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
        *background = TRUE;

    }

    // for (int i = 0; i < count; i++){
    //     printf("%s testing", tokenList[i]);
    // }
    // printf("count: %d\n", cmdLen);
    free(stringToSplitCopy);
    return tokenList;
}

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
    //declaring a sigset that contains every catchable signal except SIGCHLD
    sigset_t *sigset = (sigset_t*) malloc(sizeof(sigset_t));
    sigfillset(sigset);
    sigdelset(sigset,SIGCHLD);
    sigprocmask(SIG_SETMASK,sigset,NULL);

    struct sigaction my_sigaction; //declaring the struct that contains the pointer to sighandler with extra information and flag
    my_sigaction.sa_sigaction = sigchldhandler; //setting the sighandler 
    my_sigaction.sa_flags = SA_SIGINFO; //setting the flag to say we want more information
    my_sigaction.sa_mask = *sigset;
    sigaction(SIGCHLD,&my_sigaction,NULL); //this is like signal(), setting a handler for SIGCHLD

    processes = newList();

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

        char * commandToParse = readline("\033[1;32mprompt$ \001\e[0m\002");  
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
        
        
        if(strcmp(currentCommand[0],"jobs") == 0){
           // print(processes);
            printJobs(processes);
            for (int i = 0; i < commandLength; i++){
                free(currentCommand[i]);
            }  
            free(currentCommand);
            free(commandToParse);
            continue;
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
            free(commandToParse);
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

        if (pid == 0){
            
            setpgid(getpid(),getpid());
            if (background == FALSE){ //process should be foregrounded
                tcsetpgrp(STDIN_FILENO,getpid());
            }
            // else{
            //     tcsetpgrp(1,parentPid);

            // }
            char * args[commandLength+1];
            // printf("Command Length: %d \n", *commandLength);

            //adding null pointer to end of command string
            for (int i = 0; i < commandLength; i++){
                args[i] = currentCommand[i];
                // printf("arg %d: %s \n", i,args[i]);
            }
            args[commandLength] = NULL;
            int success = execvp(currentCommand[0],args);
            perror("Error: ");
            exit(EXIT_FAILURE);

        } else {
            Process_Props * current_process = newProcess_Props(pid, !background, commandToParse);
            add(processes, current_process); //This doesn ot work since fork creates own address space :(
            // if (background == TRUE){
            //     tcsetpgrp(1, parentPid);
            // }
            if (background == FALSE){
                waitpid(pid,&status, 0);
                tcsetpgrp(STDIN_FILENO,parentPid);
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