#ifndef COMMANDS_LIST_H
#define COMMANDS_LIST_H
// #define _GNU_SOURCE

enum change_stream {
	standard,
	add_to_file, // >>
	write_to_file,   // >
	read_from_file  // <
};


struct command_line {
	char* word;
	char* saved_file_name;
	char* opened_file_name;
	enum change_stream stream;
	char bg_mode;
	struct command_line* next;
};

struct command_List {
	struct command_line* command;
	struct command_List* next;
};

struct processed_commands;

// struct command_line* buffer_to_list(char* buffer);
struct command_List* buffer_to_list(char* buffer, struct processed_commands* history_start);

void list(struct command_line** head_adress, struct command_line** end, 
	char* buffer, int start_i, int end_i);

void free_list(struct command_line* head);

void free_cList(struct command_List* cList);

void print_command_line(struct command_line* line);

void remove_char(char *buffer, char deleting_symbole);

char** strlist(struct command_line** head_adress);


#endif
