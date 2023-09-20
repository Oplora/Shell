#ifndef COMMAND_VALIDATION_H
#define COMMAND_VALIDATION_H

int is_letter(char symbole);

int is_dote(char symbole);

char* int_to_string(int num);

char* GETUID();

char* GETPID();

void insert_variable(char** list_word, char** word);

enum ErrorType try_word(struct command_line** head_adress, 
	struct command_line** end_adress, char* buffer, int* i);

enum ErrorType try_single_quote(struct command_line** head_adress, 
	struct command_line** end_adress, char* buffer, int* i);

enum ErrorType try_double_quote(struct command_line** head_adress, 
	struct command_line** end_adress, char* buffer, int* i);

enum ErrorType try_greater(struct command_line** head_adress, 
	struct command_line** end_adress, char* buffer, int* i);

enum ErrorType try_less(struct command_line** head_adress, 
	struct command_line** end_adress, char* buffer, int* i);

enum ErrorType try_number(struct command_line** head_adress, 
	struct command_line** end_adress, char* buffer, int* i, struct processed_commands* history_start);


#endif