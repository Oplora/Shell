#ifndef COMMAND_EXECUTION_H
#define COMMAND_EXECUTION_H

void execute_command(struct command_List* cList, char** buffer_adress, struct processed_commands** history_start);

void ctrl_c_handler(int s);

void ctrl_z_handler(int s);

#endif