#ifndef __FILENODE__
#define __FILENODE__

#include <stdio.h>
#include <stdlib.h>
#include "occurrences.h"
#include "sorted-list.h"

struct fileNode {
	char *filename;
	SortedListPtr tokens;
};
typedef struct fileNode fileNode;

int compareFileNodes(void *fa, void *fb);
void destroyFileNode(void *f);
void upsertFileNode(fileNode *fn, char *token, int count);
fileNode * fileNodeFactory(char *filename, char *token, int count);
void printFileNode(fileNode *fn);

#endif
