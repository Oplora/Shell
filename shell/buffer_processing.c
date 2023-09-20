#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buffer_processing.h"
// #include "command_validation.h"


char* allocate_buff() {
	char* buffer = malloc(sizeof(char) * (COMMAND_BUFF_SIZE + 1));
	buffer[COMMAND_BUFF_SIZE] = '\0';
    return buffer;
}


void write_to_buffer(char* buffer, int* index, char symbol, enum ErrorType* flag)
{
	if(*flag == no_error) {
		if(*index == COMMAND_BUFF_SIZE) {
			printf("ERROR: Input too large.\n");
			for(int i = 0; buffer[i]; i++){
				buffer[i] = '\0';
			}
			*index = 0;
			*flag = input_too_large;
			return;	
		}
		buffer[*index] = symbol;
		(*index)++;
		if (symbol == '\n') {
			buffer[*index] = '\0';
			*index = 0;
			return;		
		}
		
	}
}


void read_from_buffer(char* buffer) {
	for(int i = 0; buffer[i]; i++){
		printf("%c", buffer[i]);
	}
	printf("\n");	
}


void add_to_history(
	struct processed_commands** history_start,
 	struct processed_commands** history_end, char* buffer)
{
	if(*history_start == NULL){
		*history_start = malloc(sizeof(struct processed_commands));
		*history_end = *history_start;
		(*history_start)->index = -1;
	}
	else {
		(*history_end)->next = malloc(sizeof(struct processed_commands));
		((*history_end)->next)->index = (*history_end)->index;
		*history_end = (*history_end)->next;
	}
	(*history_end)->index += 1;
	(*history_end)->buffer = malloc(strlen(buffer)+1);
	strcpy((*history_end)->buffer, buffer);
	(*history_end)->next = NULL;
	
}


void free_history(struct processed_commands* history_start)
{
	if (history_start == NULL){
		return;
	}
	struct processed_commands* prev = history_start;
	while (history_start!=NULL){
		prev = history_start;
		history_start = history_start->next;
		free(prev->buffer);
		// free(prev->index);
		free(prev);
	}
}

void show_history(struct processed_commands* history_start)
{
	struct processed_commands* prev = history_start;
	while(prev != NULL){
		printf("%4d	%s", prev->index, prev->buffer);
		prev = prev->next;
	}
}


