#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "command_t.h"


arg_node_t *
argFactory(char *argStr)
{
#ifdef DEBUG
	printf("Calling argFactory for %s\n", argStr);
#endif

	// declare locals
	arg_node_t *arg;
	size_t len;

	// allocate space for arg
	arg = (arg_node_t*)malloc(sizeof(arg_node_t));
	if (arg == NULL) {
		
		perror("Unable to create new arg node\n");
		return NULL;
	}
	
	// initialize next
	arg->next = NULL;

	// copy argStr into arg->arg
	len = strlen(argStr);
	arg->arg = (char*)malloc(sizeof(char) * len + 1);
	strcpy(arg->arg, argStr);

	return arg;
}

command_t *
ComCreate() {
	char *line;
	size_t len;
	char* cmdStr;
	command_t *com;
	arg_node_t *next;


	// allcoate space for the command_t
	com = (command_t*)malloc(sizeof(command_t));
	if (com == NULL) {

		perror("Unable to allocate space for command_t in ComCreate()\n");
		return NULL;
	}

	// get line of input from stdin and parse into command and arguments
	len  = 128;
	line = NULL;
	getline(&line, &len, stdin);
	if (line == NULL) {
		perror("Unable to get next line of input in ComCreate()\n");
		return NULL;
	}

	com->line = (char*)malloc(sizeof(char) * strlen(line) + 1);
	strcpy(com->line, line);

	cmdStr = strtok(line, " \n\t");
	if (cmdStr == NULL) {
		perror("No Commands given to ComCreate()\n");
		free(line);
		free(com);
		return NULL;
	}
	
	// copy cmdStr into com->command
	com->command = (char*)malloc(sizeof(char) * strlen(cmdStr) + 1);
	strcpy(com->command, cmdStr);
		
	// get the arguments
	cmdStr = strtok(NULL, " \t\n");
	com->arg = NULL;
	while (cmdStr != NULL) {

#ifdef DEBUG
		printf("arg: %s\n", cmdStr);
#endif
		next = argFactory(cmdStr);
		if (next == NULL) {
			perror("Unable parse command arg in ComCreate()\n");
			free(com->command);
			free(line);
			free(com);
			return NULL;
		}
		if (com->arg == NULL) {
			com->arg = next;
		}
		else {
			next->next = com->arg;
			com->arg = next;
		}
		cmdStr = strtok(NULL, " \t\n");
	}
	free(line);
	com->next = com->arg;
	return com;
}

void ComDestroy(command_t *com) {

	arg_node_t *tmp;
	arg_node_t *del;

	if (com == NULL) {
		return;
	}
	if (com->command) {
		free(com->command);
	}
	if (com->line) {
		free(com->line);
	}
	del = com->arg;
	while (del != NULL) {
		tmp = del;
		del = del->next;
		free(tmp->arg);
		free(tmp);
	}
	free(com);
}

char *
nextArg(command_t *com)
{
	char *nextArg;

	if (com == NULL) {
		perror("nextArg passed a NULL pointer\n");
		return NULL;
	}
	if (com->next == NULL) {
		return NULL;
	}
	nextArg = (char*)malloc(sizeof(char) * strlen(com->next->arg) + 1);
	strcpy(nextArg, com->next->arg);
	
	com->next = com->next->next;
	return nextArg;
}
void
resetIter(command_t *com)
{
	if (com == NULL) {
		perror("nextArg passed a NULL pointer\n");
		return;
	}
	com->next = com->arg;
}

void debugPrintCmds(command_t *com) {

	arg_node_t *ptr;

	// check to make sure everything is there
	if (com == NULL) {
		printf("com == NULL\n");	
		return;
	}
	if (com->command == NULL) {

		printf("com->command == NULL\n");
		return;
	}
	
	// print all on one line to check for whitespace
	printf("%s", com->command);
	ptr = com->arg;
	while (ptr != NULL) {
		if (ptr->arg == NULL) {
			printf("ptr->arg == NULL");
		}
		else {
			printf("%s", ptr->arg);
		}
		ptr = ptr->next;
	}
	printf("\n");
	
	// print on separate lines for readability
	printf("%s\n", com->command);
	ptr = com->arg;
	while (ptr != NULL) {
		if (ptr->arg == NULL) {
			printf("ptr->arg == NULL\n");
		}
		else {
			printf("%s\n", ptr->arg);
		}
		ptr = ptr->next;
	}
}
