#include <regex.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/types.h>  
#include <sys/wait.h>
#include <signal.h>

char * delimiters = " \n";
int cmdLen  = 0;

void sigStopHandler(int sig){};

void sigIntHandler(int sig){
    printf("^C\n");
}

char ** splitStringFromDelims(char * stringToSplit){
    //counting the number of arguments passed by calling strtok twice (not the most efficient :()
    char * stringToSplitCopy = malloc( sizeof(char) * (strlen(stringToSplit)+1));
    strcpy(stringToSplitCopy, stringToSplit); 
    char * token = strtok(stringToSplitCopy,delimiters);
    cmdLen = 0;
    while (token != NULL){
        cmdLen++;
        token = strtok(NULL,delimiters);
    }

    if (cmdLen == 0){ //nothing was passed
        return NULL;
    }

    char ** tokenList = (char **) malloc(sizeof(char *) * cmdLen);

    token = strtok(stringToSplit, delimiters);
    tokenList[0] = (char *) malloc(sizeof(char)* (strlen(token)+1));
    strcpy(tokenList[0],token);
    for (int i = 1; i < cmdLen; i++){
        token = strtok(NULL, delimiters);
        tokenList[i] = (char *) malloc(sizeof(char)* (strlen(token)+1));
        strcpy(tokenList[i],token);
    }

    // for (int i = 0; i < count; i++){
    //     printf("%s testing", tokenList[i]);
    // }
    // printf("count: %d\n", cmdLen);
    free(stringToSplitCopy);
    return tokenList;
}


int getNthHistory(int n, char *** currentCommand, int top){
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
        *currentCommand = splitStringFromDelims(commandCopy);
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
        int addToHistory = 1;

        char * commandToParse = readline("prompt$ \n");  
        if (!commandToParse){
            // printf("ctrl-d, exiting...\n");
            exit(0);
        } 

        char * commandCopy = malloc( sizeof(char) * (strlen(commandToParse)+1)); // making a copy because of how readline handles history
        // beacuse strtok replaces with null byte
        strcpy(commandCopy, commandToParse);

        char ** currentCommand = splitStringFromDelims(commandCopy);
        free(commandCopy);

        int * commandLength = &cmdLen; //realizing this is kind of pointless but and i shou
        if (currentCommand == NULL) continue;

        if (strcmp(currentCommand[0],"exit") == 0) {
            for (int i = 0; i < *commandLength; i++){
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
             for (int i = 0; i < *commandLength; i++){
            free(currentCommand[i]);
            }   

            free(history);
            free(currentCommand);
            free(commandToParse);;
            continue;   
        }
        if (*commandLength == 1){
            if(strcmp(currentCommand[0], "!!") == 0){
                getNthHistory(-1,&currentCommand, 0);
                addToHistory =0;
            }

            else if (regexec(&nregex,currentCommand[0],0,NULL,0) == 0){ // successful match returns 0
                addToHistory =0;
                char numberStr[strlen(currentCommand[0])];
                strcpy(numberStr, currentCommand[0]+1); // removing !
                int number = atoi(numberStr);
                int success = getNthHistory(number,&currentCommand, 1);
                if (!success){ //e.g number too high 
 
                    printf("Please enter a valid history number\n");
                    for (int i = 0; i < *commandLength; i++){
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
                int success = getNthHistory(number,&currentCommand, 0);
                if (!success){ //e.g number too high 
 
                    printf("Please enter a valid history number\n");
                    for (int i = 0; i < *commandLength; i++){
                            free(currentCommand[i]);
                    }
                        free(currentCommand);
                        free(commandToParse);
                        continue;
                }
            }
        

        }


        pid_t pid = fork();
        if (pid == 0){
            char * args[*commandLength+1];
            // printf("CommnadLen %d \n", *commandLength);
            for (int i = 0; i < *commandLength; i++){
                args[i] = currentCommand[i];
                // printf("arg %d: %s \n", i,args[i]);
            }
            args[*commandLength] = NULL;
            int success = execvp(currentCommand[0],args);
            perror("Error: ");
            exit(1);
        } else {
            waitpid(pid,&status, 0);
            if(status == 0 && addToHistory) add_history(commandToParse);
        }

        for (int i = 0; i < *commandLength; i++){
            free(currentCommand[i]);
        }
        free(currentCommand);
        free(commandToParse);
    }
}
