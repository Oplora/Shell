#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h> // for system calls
#include <limits.h> // for PATH_MAX
#include <sys/wait.h>
// for open()
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
// my modules
#include "commands_list.h"
#include "buffer_processing.h"
#include "command_validation.h"
#include "command_execution.h"

void free_all(struct command_line* head, char* cmd, char* buffer_adress, struct processed_commands* history_start)
{
	free(cmd);
	free(buffer_adress);
	free_history(history_start);
	free_list(head);
}

// void change_stream(struct command_line* head, int pid, char* buffer_adress, struct processed_commands* history_start)
// {
// 	char** cmd = strlist(&head);
// 	int status, fd;
// 	struct command_line* temp = head;
// 	while(temp->saved_file_name == NULL){
// 		temp = temp->next;
// 	}
// 	fd = open(temp->saved_file_name, O_CREAT|O_WRONLY|O_TRUNC, 0666);
// 	if(fd == -1) {
// 		perror(temp->saved_file_name);
// 		free_all(head, cmd, buffer_adress, history_start);
// 		exit(1);
// 	}
// 	pid = fork();
// 	if(pid == 0) {
// 		if(head->stream == write_to_file) dup2(fd, 1);
// 		if(head->stream == read_from_file) dup2(fd, 0);
// 		close(fd);
// 		execvp(*cmd, cmd);
// 		perror(*cmd);
// 		free_all(head, cmd, buffer_adress, history_start);
// 		exit(1);
// 	}
// 	if (head->bg_mode == 0) {
// 		current_pid = pid;
// 		while(pid != (kpid = wait(NULL))){}
// 		current_pid = 0;
// 	}
// 	free(cmd);
// 	close(fd);
// 	wait(&status);
// }


char* new_stream(struct command_line* head, char for_purpuse)
{
	struct command_line* temp = head;
	// printf("\n\t%s\n", temp->opened_file_name);
	// printf("\n\t%d\n", temp==NULL);	
	switch(for_purpuse){
		case 'o': 
			while(temp->opened_file_name == NULL && temp->next){
				temp = temp->next;
			}
			if(temp->opened_file_name){
				return temp->opened_file_name;
			}
			break;
		case 'r':
			while(temp->saved_file_name == NULL && temp->next){
				temp = temp->next;
				}
			if(temp->saved_file_name){
				return temp->saved_file_name;
			}
			break;
	}
	return NULL;

}

void change_stream(struct command_line* head) // !!!!!!!!!!!!NOT FIXED
{
	// printf("Was in change_stream()\n");
	char* opened_file_name = new_stream(head, 'o');
	char* file_name = new_stream(head, 'r');
	if(file_name || opened_file_name){
		int  fd;
		if(file_name) {
			printf("Saved file in %s\n", file_name);
			fd = open(file_name, O_CREAT|O_WRONLY|O_TRUNC, 0666);
			if(fd == -1) {
				perror(file_name);
				exit(1);
			}
			if(head->stream == write_to_file) dup2(fd, 1);
		}
		if(opened_file_name) { // !!!!!!!!!!!!NOT WORKING
			printf("Opened file in %s\n", opened_file_name);
			fd = open(opened_file_name, O_NONBLOCK);
			if(fd == -1) {
				perror(opened_file_name);
				exit(1);
			}
			if(head->stream == read_from_file) dup2(fd, 0);	
		}
		close(fd);
		// return fd;
	}
	// return -24;
}

volatile static sig_atomic_t current_pid = 0;

void ctrl_c_handler(int s){
	signal(SIGINT, ctrl_c_handler);
	// printf("\nWas in ctrl_c_handler\n");
	if(s == SIGINT && current_pid){
		// printf("CUR_PID = %d\n", current_pid);
		kill(current_pid, SIGINT);
		current_pid = 0;
	} 
}

// void ctrl_z_handler(int s){
// 	signal(SIGTSTP, ctrl_z_handler);
// 	// printf("\nWas in ctrl_z_handler\n");
// 	if(s == SIGTSTP && current_pid){
// 		// printf("CUR_PID = %d\n", current_pid);
// 		// kill(current_pid, SIGSTOP);
// 		// current_pid = 0;
// 		// printf("Welcome to shell!\n-->");
// 	} 
// }


void run_utility(struct command_line* head, char* buffer_adress, struct processed_commands* history_start)
{
	char** cmd = strlist(&head);
	int pid = fork();
	
	int kpid = 0; // pID from wait()
	if (pid == 0) {

		// int stream = change_stream(head);
		// if(stream != -24) close(stream);
		change_stream(head);

		execvp(*cmd, cmd);
		perror(*cmd);
		// free(cmd);
		free(cmd);
		free(buffer_adress);
		free_history(history_start);
		free_list(head);
		exit(1);
	}
	if (head->bg_mode == 0) {
		// printf("WAS IN FG"); 
		current_pid = pid;
		// printf("pid = %d , current_pid = %d\n", pid, current_pid);
		while(pid != (kpid = wait(NULL)))
		{
			// printf("###CATCHED PROC WITH ID %d\n. WAITING %d\n",kpid, pid);
		} // Ждем пока завершится процесс
		current_pid = 0;
	}
	// printf("###CATCHED PROC WITH ID %d\n. NEEDED %d\n",kpid, pid);
	free(cmd);
}

int custom_commands(struct command_line* head, char* buffer_adress, struct processed_commands* history_start)
{
	if(!strcmp(head->word, "cd")) {
		if (chdir((head->next)->word)) perror((head->next)->word);
		return 1;
	} 
	else if(!strcmp(head->word, "exit")) {
		free_list(head);
		free(buffer_adress);
		exit(0);
	} 
	else if(!strcmp(head->word, "history")){
		show_history(history_start);
		return 1;
	}
	return 0;
}

void execute_command(struct command_List* cList, char** buffer_adress, struct processed_commands** history_start)
{
	while(cList != NULL){
		struct command_line* head = cList->command;
		if(head != NULL) {
			if(!custom_commands(head, *buffer_adress, *history_start)) run_utility(head, *buffer_adress, *history_start);
		}
		cList = cList->next;
	}
}
	