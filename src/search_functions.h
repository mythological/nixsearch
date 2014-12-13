#ifndef __SEARCH_FUNCTIONS__
#define __SEARCH_FUNCTIONS__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "fileNode.h"
#include "command_t.h"
#include "inv_index_t.h"
#include "run_indexer.h"

SortedListPtr merge_and(SortedListPtr sla, SortedListPtr slb);
SortedListPtr merge_or(SortedListPtr sla, SortedListPtr slb);
void print_results(SortedListPtr sl);
void search_and(inv_index_t *index, command_t *com);
void search_or(inv_index_t *index, command_t *com);
void display_help(command_t *com);
void display_commands(void);
void changeDir(command_t *com);
inv_index_t * newIndex(command_t *com, inv_index_t *index);
inv_index_t * use_indexer(command_t *com, inv_index_t *index);
#endif
