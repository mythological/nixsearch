#ifndef __INV_INDEX__
#define __INV_INDEX__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>

#include "cfuhash.h"
#include "cfutypes.h"
#include "sorted-list.h"
#include "occurrences.h"

struct inv_index_t {
	cfuhash_table_t *hash;
	char *index_file;
};
typedef struct inv_index_t inv_index_t;

inv_index_t* load_index(char *filename);
void destroy_index(inv_index_t *index);
int validateIndex(char *inputFile);
#endif
