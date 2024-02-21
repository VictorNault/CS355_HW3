node.o: node.h
	gcc -c node.c
List.o: node.h node.c List.h
	gcc -c List.c
List_Extras.o: node.h List_Extras.h
	gcc -c List_Extras.c
Process_Props.o: common.h Process_Props.h
	gcc -c Process_Props.c
commands.o: List.h Process_Props.h node.h
	gcc -c commands.c
kais_shell.o: common.h Process_Props.h List.h commands.h
	gcc -c kais_shell.o
kais_shell: Process_Props.o List.o commands.o
	gcc -o kais_shell Process_Props.o List.o commands.o kais_shell.o
