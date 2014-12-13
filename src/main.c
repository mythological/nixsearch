#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "inv_index_t.h"
#include "command_t.h"
#include "search_functions.h"


#ifndef __TRUE_FALSE__
#define TRUE  1
#define FALSE 0
#endif

#ifndef __ANSI_COLORS__
#define ANSI_COLOR_RED   "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_CYAN  "\x1b[36m"
#define ANSI_COLOR_WHITE "\x1b[37m"
#define ANSI_COLOR_RESET "\x1b[0m"
#endif

void endOfJob(inv_index_t *index);
inv_index_t * menu(inv_index_t *index);

int
main(int argc, char **argv)
{
	inv_index_t *index = NULL;

	// prepare prompt
	printf(ANSI_COLOR_WHITE);

	if (argc > 1) {
		printf("Loading the inverted index file.\n\n");

		// load the index
		index = load_index(argv[1]);
		if (index == NULL) {
			printf("Unable to load inverted index.\n");
		}
	}
	
	// enter the main menu	
	index = menu(index);

	endOfJob(index);
	return 0;
}

inv_index_t* 
menu(inv_index_t *index)
{
	int          quit    = FALSE;
	command_t   *com     = NULL;
	char        *pwd     = NULL;
	size_t       pwd_len = 32;

	printf("\nWelcome to Search.\n");
	printf("Type a command, then hit the return key.\n\n");
	printf("Type 'help <command>' for instructions on how to a particular\n");
	printf("command. Type 'commands' or 'c' for a list of commands. Type\n");
	printf("'quit' or 'q' to quit.\n\n");


	// read commands until user quits
	while (quit == FALSE) {
	
		
		// get the current working directory
		pwd = getcwd(pwd, pwd_len);
		while (pwd == NULL) {
			pwd_len *= 2;
			pwd = getcwd(pwd, pwd_len);
		}
	
		// display the prompt
		printf("[ " ANSI_COLOR_CYAN "%s" ANSI_COLOR_WHITE " ]\n", pwd);
		printf("search ");	
			printf("( ");
		if (index) {
			printf(ANSI_COLOR_GREEN);
			printf("%s", index->index_file);
		}
		else {
			printf(ANSI_COLOR_RED);
			printf("load index file");
		}
		printf(ANSI_COLOR_WHITE);
		printf(" ) ");
		printf("> ");	

		com = ComCreate();

		if (com == NULL) {
			continue;
		}
		else if ( !(strcmp(com->command, "q")   ) ||
				  !(strcmp(com->command, "quit"))   ) {
						quit = TRUE;
		}
		else if ( !(strcmp(com->command, "sa")) ) {
			search_and(index, com);
		}	
		else if ( !(strcmp(com->command, "so")) ) {
			search_or(index, com);
		}	
		else if ( !(strcmp(com->command, "cls")   ) ||
				  !(strcmp(com->command, "clear"))   ) {
			system("clear");
		}	
		else if ( !(strcmp(com->command, "ls")) ) {
			system(com->line);
		}	
		else if ( !(strcmp(com->command, "cd")) ) {
			changeDir(com);
		}	
		else if ( !(strcmp(com->command, "l")   ) ||
				  !(strcmp(com->command, "load"))   ) {
			index = newIndex(com, index);
		}	
		else if ( !(strcmp(com->command, "h")   ) ||
				  !(strcmp(com->command, "help"))   ) {
			display_help(com);
		}
		else if ( !(strcmp(com->command, "c")   ) ||
				  !(strcmp(com->command, "commands"))   ) {
			display_commands();
		}
		else if ( !(strcmp(com->command, "i")   ) ||
				  !(strcmp(com->command, "index"))   ) {
			index = use_indexer(com, index);
		}	
		ComDestroy(com);
		free(pwd);
		pwd = NULL;
	}
	return index;
}

void
endOfJob(inv_index_t *index)
{
	if (index != NULL) {
		destroy_index(index);
	}
	printf("Goodbye!\n");
	printf(ANSI_COLOR_RESET);
}
