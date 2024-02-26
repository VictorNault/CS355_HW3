Name: Ben Jiang, Kai Britt, Victor Nault

Instructions for compiling:
run the following commands:
make clean
make kais_shell

How to run:ommands.
run the following command:
./kais_shell

Known bugs and limitation:

Features and extra credit completed:
No extra features completed. 

File Directory:
commands.c/h: containing code for built in functions fg, bg, jobs, kill
design.c: our original design
kais_shell.c: contains the parser, uses readline that takes command from commandline, forks off a child, and runs command
List_Extras.c/h: contains methods for the LinkedList for the ProcessProps struct, such as deleting a ProcessProp, or finding it by pid or jobid
List.c/h: contains implementation and methods for a linked list data structure
node.c/h: contains implementation and methods for a node data structure
Process_Props.c/h: contains implementation and methods for Process_Props struct, containing flags relevant to a process
Sighandlers.c: contains the sigchldhandler that catches any SIGCHLD sent from children processes and updates the linked list
string_extras.c/h: contains a function to trim strings

