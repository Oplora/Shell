#ifndef BUFFER_PROCESSING_H
#define BUFFER_PROCESSING_H

#define COMMAND_BUFF_SIZE 64
enum ErrorType {
	no_error,

	input_too_large,

	no_space_before_single_quote,
	sinle_quote_not_closed,
	double_quote_not_closed,
	no_space_before_double_quote,

	no_space_before_slash,
	invalid_symbole,
	no_space_before_semicolon,

	no_such_event_in_history,
	invalid_command_number,

	no_space_before_symbole_greater,
	no_space_after_symbole_greater,
	no_word_after_greater_symbole,
	too_many_words_after_greater_symbole,

	no_space_before_symbole_less,
	no_space_after_symbole_less,
	no_word_after_less_symbole,
	too_many_words_after_less_symbole
};

struct processed_commands {
	int index;
	char* buffer;
	struct processed_commands* next;
};

char* allocate_buff();

void write_to_buffer(char* buffer, int* index, char symbol, enum ErrorType* flag);

void read_from_buffer(char* buffer);

void add_to_history(
	struct processed_commands** history_start,
 	struct processed_commands** history_end, char* buffer);

// void make_history(char** buffer);

void free_history(struct processed_commands* history_start);

void show_history(struct processed_commands* history_start);



#endif
