#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // for system calls
#include <limits.h> // for PATH_MAX
#include <sys/wait.h>
#include "commands_list.h"
#include "buffer_processing.h"
#include "command_validation.h"



char* word_from_buffer(char** buffer, int start_index, int end_index)
{
	int word_len = end_index - start_index;
	char* word = malloc(sizeof(char) * word_len + 1);
	strncpy(word, &((*buffer)[start_index]), word_len);
	word[word_len] = '\0';
	return word;
}

// Adds new word to list or build a first one
struct command_line* expand_list( struct command_line** end, char* buffer,
 int start_i, int end_i)
{	
	if(*end == NULL) {
		*end = malloc(sizeof(struct command_line));
		(*end)->bg_mode = 0;
		(*end)->stream = standard;
	}
	else {
		if((*end)->stream == standard) {

			(*end)->next = malloc(sizeof(struct command_line));
			((*end)->next)->stream = (*end)->stream;
			(*end) = (*end)->next;
		}
	}
	char* word = word_from_buffer(&buffer, start_i, end_i);

	(*end)->saved_file_name = NULL;
	(*end)->opened_file_name = NULL;
	
	if((*end)->stream == write_to_file) {
		insert_variable(&((*end)->saved_file_name), &word);
	}
	else if((*end)->stream == read_from_file) {
		insert_variable(&((*end)->opened_file_name), &word);
	}
	else {
		insert_variable(&((*end)->word), &word);
	}
	
	(*end)->next = NULL;
	return (*end);
}

// Trigger expand_list() function with consideration of header pointer state
void list(struct command_line** head_adress, struct command_line** end, 
	char* buffer, int start_i, int end_i) 
{
	if (*head_adress == NULL) {
		*head_adress = expand_list(end, buffer, start_i, end_i);
	}
	else {
		*end = expand_list(end, buffer, start_i, end_i);
	}
}


void free_list(struct command_line* head)
{
	if (head == NULL){
		return;
	}
	struct command_line* prev = head;
	while (head != NULL){
		prev = head;
		// printf("List is free\n");
		head = head->next;
		free(prev->word);
		// free(prev->bg_mode);
		free(prev);
	}
}

int list_len(struct command_line* head) 
{
	int words_counter = 0;
	for(; head; head = head->next){
		words_counter++;
	}
	return words_counter;
}

void error_notification(enum ErrorType flag, struct command_line** head)
{
	if(flag != no_error) {
		switch(flag) {
			case double_quote_not_closed:
				printf("Error:\n*** You haven't closed a quote \"");
				break;
			case sinle_quote_not_closed:
				printf("Error:\n*** You haven't closed a quote \'");
				break;
			case no_space_before_double_quote:
				printf("Error:\n*** You haven't typed space before a quote \"");
				break;
			case no_space_before_single_quote:
				printf("Error:\n*** You haven't typed space before a quote \'");
				break;
			case no_space_before_slash:
				printf("Error:\n*** You haven't typed space before a slash \\");
				break;
			case invalid_symbole:
				printf("ErRoR:\n*** Unknown symbole occured in buffer.");
				break;
			case no_space_before_semicolon:
				printf("Error:\n*** You haven't typed space before a semicolon ;");
				break;
			case no_space_after_symbole_greater:
				printf("Error:\n*** You haven't typed space after a symbole >");
				break;
			case no_space_before_symbole_greater:
				printf("Error:\n*** You haven't typed space before a symbole >");
				break;
			case no_word_after_greater_symbole:
				printf("Error:\n*** You haven't typed word after a symbole >");
				break;
			case too_many_words_after_greater_symbole:
				printf("Error:\n*** You typed too many words after a symbole >");
				break;
			case invalid_command_number:
				printf("Error:\n*** Command number should consists of numbers");
				break;
			case no_such_event_in_history:
				printf("Error:\n*** No such event in history >");
				break;
			default:
				break;
		}
		printf("\n");
		free_list(*head);
		*head = NULL;
	}
}


void add_to_command_List(struct command_List** cList,
 struct command_line** head, struct command_line** end)
{
	if(*head){
		// struct command_line* head_copy = malloc(sizeof(struct command_line));
		// head_copy = *head;
		if (*cList == NULL) {
			(*cList) = malloc(sizeof(struct command_List));
			// (*cList)->command = malloc(sizeof(struct command_line));
			// (*cList)->command = head_copy;
			(*cList)->command = *head;
			(*cList)->next = NULL;
			// free_list(*head);
			// free_list(*end);
			*head = NULL;
			*end = NULL;
		}
		else{
			add_to_command_List(&((*cList)->next), head, end);
		}
	}
}


void free_cList(struct command_List* cList)
{
	if (cList == NULL){
		return;
	}
	struct command_List* prev = cList;
	while (cList!=NULL){
		prev = cList;
		cList = cList->next;
		free_list((prev->command));
		free(prev);
	}
}

void show_cList(struct command_List* cList){
	struct command_List* prev = cList;
	printf("\n	In command list:\n");
	while(prev != NULL) {
		printf("command: ");
		print_command_line((prev->command));
		prev = prev->next;
	}
	printf("\n	-----------\n");
}

// Main processing function
// struct command_line* buffer_to_list(char* buffer)
struct command_List* buffer_to_list(char* buffer, struct processed_commands* history_start)
{
	struct command_List *cList = NULL;
	struct command_line *head = NULL, *end = NULL;
	enum ErrorType flag = no_error;

	for(int i = 0; buffer[i] && flag == no_error; i++){
		switch(buffer[i]) {
			case '#':
				i = COMMAND_BUFF_SIZE;
			case '\n':
				// add_to_command_List(&cList, &head, &end);
				// greet();
				break;
			case ' ':
				break;
			case '\'':
				flag = try_single_quote(&head, &end, buffer, &i);
				break;
			case '\"':
				flag = try_double_quote(&head, &end, buffer, &i);
				break;
			case '\\':
				break;
			case '&':
				if(head != NULL) head->bg_mode = 1;
				break;
			case ';':
				add_to_command_List(&cList, &head, &end);
				break;
			case '>':
				flag = try_greater(&head, &end, buffer, &i);
				break;
			case '<':
				flag = try_less(&head, &end, buffer, &i);
				break;
			case '!':
				flag = try_number(&head, &end, buffer, &i, history_start);
				break;
			default:
				if(is_letter(buffer[i]) || is_dote(buffer[i])) {
					flag = try_word(&head, &end, buffer, &i);
					// printf("In main body. head: %x\n", head);
					// printf("In main body. i: %d\n", i);
				}
				else flag = invalid_symbole;
		}
	}
	add_to_command_List(&cList, &head, &end);
	show_cList(cList);
	// free_cList(cList);
	error_notification(flag, &head);
	return cList;
	// return head;
}


char** strlist(struct command_line** head_adress) 
{
	int len = list_len(*head_adress);
	char** pointers_arr = malloc(sizeof(char*)*(len+1));
	struct command_line* temp = *head_adress;
	for(int i = 0; temp; temp = temp->next){
		pointers_arr[i] = temp->word;
		i++;
	}
	pointers_arr[len] = NULL;
	return pointers_arr;
}


void print_command_line(struct command_line* head)
{
	struct command_line* line = head;
	while(line != NULL){
		printf("[%s]", line->word);
		line = line->next;
	}
	printf("\n");
}

