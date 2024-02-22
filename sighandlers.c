#include "sighandlers.h"

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