#include "common.h"

char * delimiters = " \n";
pid_t shellPid;
List * processes;
struct termios shellTermios;

// to do remove leading and trailing white spaces
char ** splitSemiColon(char * stringToSplit, int * numCmds){
    char * stringToSplitCopy = malloc( sizeof(char) * (strlen(stringToSplit)+1));
    strcpy(stringToSplitCopy, stringToSplit); 
    char * token = strtok(stringToSplitCopy,";");
    *numCmds = 0;
    while (token != NULL){
        (*numCmds)++;
        token = strtok(NULL,";");
    }
    if (*numCmds == 0){ //nothing was passed
        return NULL;
    }
    free(stringToSplitCopy);

    char ** commands = malloc(sizeof(char*) * (*numCmds));
    stringToSplitCopy = malloc( sizeof(char) * (strlen(stringToSplit)+1));
    strcpy(stringToSplitCopy, stringToSplit); 
    token = strtok(stringToSplitCopy,";");
    commands[0] = malloc(sizeof(char) * strlen(token)+1);
    strcpy(commands[0], token);
    for (int i = 1; i < *numCmds; i++){
        token = strtok(NULL,";");
        commands[i] = malloc(sizeof(char) * strlen(token)+1);
        strcpy(commands[i], token);
    }
    free(stringToSplitCopy);
    return commands;

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
        free(tokenList[(*cmdLen)-1]); //freeing malloced &
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

int getNthHistory(int n, char *** currentCommand, int top, int * cmdLen, int * background){
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
        for (int i = 0; i < *cmdLen; i++){
        free(*currentCommand[i]);
        }

        free(*currentCommand);
        printf("%s\n",commandCopy);
        *currentCommand = splitStringFromDelims(commandCopy, cmdLen, background);
        for (int i = 0; i < *cmdLen; i++){
            // printf("\n%d: %s\n",i,currentCommand[i]);
        }
        free(commandCopy);
        free(history);
        return 1;
}

int main(){
    int status;
    shellPid = getpid();

    //declaring a sigset that contains every catchable signal except SIGCHLD
    sigset_t *sigset = (sigset_t*) malloc(sizeof(sigset_t));
    sigfillset(sigset);
    sigdelset(sigset,SIGCHLD);
    sigprocmask(SIG_BLOCK,sigset,NULL);

    struct sigaction my_sigaction; //declaring the struct that contains the pointer to sighandler with extra information and flag
    my_sigaction.sa_sigaction = sigchldhandler; //setting the sighandler 
    my_sigaction.sa_flags = SA_SIGINFO | SA_RESTART; //setting the flag to say we want more information
    my_sigaction.sa_mask = *sigset;
    sigaction(SIGCHLD,&my_sigaction,NULL); //this is like signal(), setting a handler for SIGCHLD

    tcgetattr(STDIN_FILENO,&shellTermios); //getting shell's termios

    processes = newList();
    free(sigset);
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
    while(TRUE){
        int addToHistory = TRUE;
        // printf("\033[1;32m%s$\001\e[0m\002", getenv("USER")); // trying different prompt string
        char * commandToParse = readline("\033[1;32mprompt$ \001\e[0m\002"); 
        if (!commandToParse){
            printf("\n");
            sigset = (sigset_t*) malloc(sizeof(sigset_t));
            sigfillset(sigset);
            sigprocmask(SIG_BLOCK,sigset,NULL);
            free(sigset);
            clear(processes);

            HISTORY_STATE * history = history_get_history_state();
            HIST_ENTRY ** histlist = history_list();

            for (int i = 0; i < history->length; i++){
                free_history_entry(histlist[i]);
            }
            free(history);
            regfree(&nregex);
            regfree(&dashNRegex);
            exit(EXIT_SUCCESS);
        } 

        // if (commandToParse == NULL){
        //     free(commandToParse);
        //     continue;
        // }


        int numCmds;
        char ** commandList = splitSemiColon(commandToParse, &numCmds);
        for (int i = 0; i < numCmds; i++){
        char * trimmedCommand = trimStr(commandList[i]);
        free(commandList[i]);
        commandList[i] = trimmedCommand;
        add_history(commandList[i]);
        
        char * commandCopy = malloc( sizeof(char) * (strlen(commandList[i])+1)); // making a copy because of how readline handles history
        // beacuse strtok replaces with null byte
        strcpy(commandCopy, commandList[i]);
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
            free(commandList[i]);
            sigset = (sigset_t*) malloc(sizeof(sigset_t));
            sigfillset(sigset);
            sigprocmask(SIG_BLOCK,sigset,NULL);
            free(sigset);

            clear(processes);

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
            free(commandList[i]);
            continue;
        }

        if(strcmp(currentCommand[0],"fg") == 0){
            int job_number;

            //printf("%d",currentCommand[1]);
            if(commandLength == 1){
                fg(-1); //-1 means most recent, implemented in the fg function
            }else if (commandLength == 2){
                if ('%' == *currentCommand[1]){
                    if (strlen(currentCommand[1]) == 1)
                    {
                        fg(-1);
                    }                    
                    else
                    {
                    char * temp = currentCommand[1] + 1;
                        job_number = atoi(temp);
                        if (job_number > 0){
                            fg(job_number);    
                        }
                        else{
                        printf("\033[0;31mError:\001\e[0m\002 Invalid job_id\n");
                        } 
                }
            }
            else{ // just an number was passed
                job_number = atoi(currentCommand[1]);
                if (job_number > 0){
                    fg(atoi(currentCommand[1]));
                }
                else{
                    printf("\033[0;31mError:\001\e[0m\002 Invalid job_id\n");
                }
            }
            }
            else{
                printf("\033[0;31mError:\001\e[0m\002 Invalid call to bg\n");
            }
            
            for (int i = 0; i < commandLength; i++){
                free(currentCommand[i]);
            }  
            free(currentCommand);
            free(commandList[i]);
            continue;
        }

        if(strcmp(currentCommand[0],"bg") == 0){
            int job_number;
            if(commandLength == 1){
                bg(-1); //-1 means most recent, implemented in the fg function
            }
            else if (commandLength == 2)
            {
                if ('%' == *currentCommand[1]){
                    if (strlen(currentCommand[1]) == 1){
                        bg(-1);
                    }
                    else{
                        char * temp = currentCommand[1] + 1;
                        job_number = atoi(temp);
                        if (job_number > 0){
                            bg(job_number);    
                        }
                        else{
                            printf("\033[0;31mError:\001\e[0m\002 Invalid job_id\n");
                        }
                    }
                }
                else{
                    job_number = atoi(currentCommand[1]);
                    if (job_number > 0){
                        bg(atoi(currentCommand[1]));
                    }
                    else{
                        printf("\033[0;31mError:\001\e[0m\002 Invalid job_id\n");
                    }
                }
            
            }
            else{
                printf("\033[0;31mError:\001\e[0m\002 Invalid call to bg\n");
            }
            for (int i = 0; i < commandLength; i++){
                free(currentCommand[i]);
            }  
            free(currentCommand);
            free(commandList[i]);
            continue;
        }
        
          if(strcmp(currentCommand[0],"kill") == 0){
            int killStatus;
            if(commandLength == 1){
                printf("\033[0;31mError:\001\e[0m\002 Kill needs more than 1 parameter\n"); //-1 means most recent, implemented in the fg function
            }
            else if(commandLength == 2){
                int job_to_kill = atoi(currentCommand[1]);
                if (job_to_kill <= 0){
                    printf("\033[0;31mError:\001\e[0m\002 Invalid job_id\n");
                    for (int i = 0; i < commandLength; i++){
                        free(currentCommand[i]);
                    }  
                    free(currentCommand);
                    free(commandList[i]);
                    continue;
                }
                killStatus = myKill(job_to_kill, FALSE); // sending sig term
                if (killStatus == -1){
                    perror("\033[0;31mError\001\e[0m\002");
                }
            }
            else if(commandLength == 3){
               if (strcmp(currentCommand[1],"-9") == 0 && atoi(currentCommand[2]) != 0){
                    killStatus= myKill(atoi(currentCommand[2]),TRUE);
                if (killStatus == -1){
                    perror("\033[0;31mError\001\e[0m\002");
                }
                }
            else{
                printf("\033[0;31mError:\001\e[0m\002 Invalid call to kill\n");
            }
            }
            
            for (int i = 0; i < commandLength; i++){
                free(currentCommand[i]);
            }  
            free(currentCommand);
            free(commandList[i]);
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
            free(commandList[i]);
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
                        free(commandList[i]);
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
                        free(commandList[i]);
                        continue;
                }
            }
        

        }
        

        pid_t pid = fork();

        if (pid == 0){
            setpgrp();
            sigset_t newset;
            sigemptyset(&newset);
            sigprocmask(SIG_SETMASK,&newset,NULL);
            char * args[commandLength+1];
            // printf("Command Length: %d \n", *commandLength);

            //adding null pointer to end of command string
            for (int i = 0; i < commandLength; i++){
                args[i] = currentCommand[i];
                // printf("arg %d: %s \n", i,args[i]);
            }
            args[commandLength] = NULL;
            int success = execvp(currentCommand[0],args);
            
            perror("\033[0;31mError\001\e[0m\002");
            exit(EXIT_FAILURE);

        } else {
            setpgid(pid,pid);
            Process_Props * current_process = newProcess_Props_nt(pid, !background,commandList[i]);
            add(processes, current_process); //This doesn ot work since fork creates own address space :(
            
            if (background == TRUE){
                tcsetpgrp(STDIN_FILENO,shellPid);
                printf("[%d]    %d\n",current_process->job_id,current_process->pid);
                waitpid(pid,&status,WNOHANG|WUNTRACED);
            }
            else{
                tcsetpgrp(STDIN_FILENO,pid);
                waitpid(pid,&status,WUNTRACED);
                current_process->hasTermios = TRUE;
                tcsetattr(STDIN_FILENO, TCSADRAIN ,&shellTermios);
                tcsetpgrp(STDIN_FILENO,shellPid);
            }
            
            // add_history(commandList[i]);
        }

        for (int i = 0; i < commandLength; i++){
            free(currentCommand[i]);
        }
        free(currentCommand);
        free(commandList[i]);
        }
        free(commandList);
        free(commandToParse);
    }
}