#include "sighandlers.h"

void sigchldhandler(int signal, siginfo_t *info, void *ucontext){
    if(info->si_code == CLD_EXITED || info->si_code == CLD_DUMPED || info->si_code == CLD_KILLED){ //si_code in the info struct contains if the child is suspended, exited, or interrupted...
        //Entering critical region
        waitpid(info->si_pid,NULL,WNOHANG);
        pthread_mutex_lock(&mutex);
        //***delete child from ll
        printf("Child has exited or dumped or killed\n");
        pthread_mutex_unlock(&mutex);
    }
    else if(info->si_code == CLD_STOPPED){
        //Entering critical region
        pthread_mutex_lock(&mutex);
        //***update flag in node
        printf("Child has suspended\n");
        pthread_mutex_unlock(&mutex);
    }
    else if(info->si_code == CLD_CONTINUED){
        //Entering critical region
        pthread_mutex_lock(&mutex);
        //***update flag in node
        printf("Child has continued\n");
        pthread_mutex_unlock(&mutex);
    }
}