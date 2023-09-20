#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "commands_list.h"
#include "buffer_processing.h"
#include "command_validation.h"
#include "command_execution.h"


void end_shell(char* buffer, struct processed_commands *history){
	free_history(history);
    free(buffer);    
}

int start_shell(char* buffer){
    char c = 0;
	int i = 0; // use for counting char index in string
	enum ErrorType flag = no_error;
	struct processed_commands *history_start = NULL; 
	struct processed_commands *history_end = NULL;
	signal(SIGINT, ctrl_c_handler);

    printf("Welcome to shell!\n-->");
    while((c = getchar()) != EOF){
		write_to_buffer(buffer, &i, c, &flag);
		if (c == '\n') {
			add_to_history(&history_start, &history_end, buffer);
			struct command_List* cList = buffer_to_list(buffer, history_start);
			// struct command_line* comline = buffer_to_list(buffer);
			// print_command_line(comline);	
			execute_command(cList, &buffer, &history_start);
			// show_history(history_start);
			// free_list(comline);
			free_cList(cList);
			flag = no_error;
		    printf("-->");
		}

    }
    end_shell(buffer, history_start);
    return 0;
}


int main()
{
    char* buffer = allocate_buff();
	return start_shell(buffer);

}
