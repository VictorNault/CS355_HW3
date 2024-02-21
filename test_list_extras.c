#include <stdio.h>

#include "common.h"
#include "List_Extras.h"

#define MYPID1 11
#define MYPID2 22

int main() {
    List * my_list = newList();
    Process_Props * my_props1 = newProcess_Props(MYPID1, 1, "my_command1");
    Process_Props * my_props2 = newProcess_Props(MYPID2, 0, "my_command2");
    add(my_list, my_props1);
    add(my_list, my_props2);

    Process_Props * found_process = get_by_pid(my_list, MYPID1);
    printf("Found process with pid %d\n", found_process->pid);
    return 0;
}
