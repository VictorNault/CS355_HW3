#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

#define DELIMITERS_STR " \n"

int main () {
    while (1) {
        char * shell_inp = readline("Welcome to MyShell. Please input a command: ");
        // Because strtok butchers the original string while processing it...
        char * shell_inp_cpy = malloc(strlen(shell_inp) + 1);
        strcpy(shell_inp_cpy, shell_inp);
        char * next_tok = strtok(shell_inp_cpy, DELIMITERS_STR);
        int toks_count = 1;
        while ((next_tok = strtok(NULL, DELIMITERS_STR)) != NULL) {
            toks_count++;
        }
        // Free the mem used for the shell input copy used to count number of tokens
        free(shell_inp_cpy);
        char * toks_arr[toks_count];
        toks_arr[0] = strtok(shell_inp, DELIMITERS_STR);
        // If input is empty, just try again
        if (toks_arr[0] != NULL) {
            // toks_count + 1 so a NULL pointer is put at the end of the array; needed for execvp() call
            for (int i = 1; i < toks_count + 1; i++) {
                toks_arr[i] = strtok(NULL, DELIMITERS_STR);
            }
            if (strcmp("exit", toks_arr[0]) == 0) {
                free(shell_inp);
                exit(EXIT_SUCCESS);
            }
            else {
                int exec_ret;
                pid_t pid;
                pid = fork();
                if (pid == -1) {
                    printf("Error - failed to fork, error code %d\n", pid);
                }
                if (pid == 0) {
                    exec_ret = execvp(toks_arr[0], toks_arr);
                    if (exec_ret == -1) {
                        printf("Error, execvp() has failed, error code %d\n", errno);
                        exit(EXIT_FAILURE);
                    }
                    exit(EXIT_SUCCESS);
                }
                if (pid > 0) {
                    wait(NULL);
                }
            }
        }
        free(shell_inp);
        free(next_tok);
    }
    return 0;
}
