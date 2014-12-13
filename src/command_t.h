#include <stdio.h>
#include <stdlib.h>

#ifndef __COMMAND_T__
#define __COMMAND_T__

struct arg_node_t {
	struct arg_node_t *next;
	char *arg;
};
typedef struct arg_node_t arg_node_t;

struct command_t {
	char *command;
	char *line;
	arg_node_t *arg;
	arg_node_t *next;
};
typedef struct command_t command_t;

arg_node_t *argFactory(char *argStr);
command_t *ComCreate();
char* nextArg(command_t *com);
void resetIter(command_t *com);
void ComDestroy(command_t *com);
void debugPrintCmds(command_t *com);

#endif
