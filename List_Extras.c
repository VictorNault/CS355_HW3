#include <List_Extras.h>

int delete_process(List *l, Process_Props * process) {
    // Check if the head is the process in question
    if ((l->head) == NULL) {
        return -1;
    }
    if ((n->data->pid) == (process->pid)) {
        // TODO terminate process
        l->head = l->head->next;
        return 0;
    }
    Process_Props * node_data;
    struct node *last_n = l->head;
    struct node *n = l->head->next;

    while (n != NULL) {
        node_data = n->data;
        if (node_data->pid == process->pid) {
            // TODO terminate process
            last_n->next = n->next;
            return 0;
        }
        last_n = n;
        n = n->next;
    }
    // If the process is not found in the list then return error code -1
    return -1;
}

Process_Props * get_by_pid(const List *l, pid_t pid) {
    struct node *n = l->head;
    pid_t * node_data;
    while (n != NULL) {
        node_data = n->data;
        if (*node_data == pid) {
            return node_data;
        }
        n = n->next;
    }
    // If the process cannot be found, return NULL
    return NULL;
}
