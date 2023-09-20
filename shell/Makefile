commands_list.o: commands_list.c commands_list.h
	gcc -g -Wall -c commands_list.c -o commands_list.o

buffer_processing.o: buffer_processing.c buffer_processing.h
	gcc -g -Wall -c buffer_processing.c -o buffer_processing.o

command_validation.o: command_validation.c command_validation.h
	gcc -g -Wall -c command_validation.c -o command_validation.o

command_execution.o: command_execution.c command_execution.h
	gcc -g -Wall -c command_execution.c -o command_execution.o

shell: shell.c commands_list.o buffer_processing.o command_validation.o command_execution.o
#	gcc -g -Wall shell.c buffer_processing.o -o shell
	gcc -g -Wall shell.c commands_list.o buffer_processing.o command_validation.o command_execution.o -o shell
	###CODE RUN###

