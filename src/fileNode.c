#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileNode.h"
#include "occurrences.h"
#include "sorted-list.h"


fileNode *
fileNodeFactory(char *filename, char *token, int count)
{
	size_t    len;
	fileNode *fn;
	SortedListPtr tokList;
	Occurrences *tok;

	if (filename == NULL || token == NULL || count == 0) {
		perror("Null or zero parameter passed to fileNodeFactory\n");
		return NULL;
	}

	// allocate space for the fileNode
	fn = (fileNode*)malloc(sizeof(fileNode));
	if (fn == NULL) {

		perror("Unable to allocate space for fileNode in fileNodeFactory\n");
		return NULL;
	}

	// copy filename into fn->filename
	len = strlen(filename);
	fn->filename = (char*)malloc(sizeof(char) * len + 1);
	if (fn->filename == NULL) {
		perror("Unable to allocate space for fn->filename in fileNodeFactory\n");
		free(fn);
		return NULL;
	}
	strcpy(fn->filename, filename);

	// pair token with count
	tok = OccurrencesFactory(token, count);
	if (tok == NULL) {
		perror("Unable to allocate space for fn->filename in fileNodeFactory\n");
		free(fn);
		return NULL;
	}
	
	
	// create sorted list of tokens and insert the token
	tokList = SLCreate(compareOccurrences, destroyOccurrences);
	if (tokList == NULL) {
		perror("Unable to create new tokList in fileNodeFactory\n");
		destroyOccurrences(token);
		free(fn->filename);
		free(fn);
		return NULL; }
	SLInsert(tokList, tok);
	fn->tokens = tokList;

	return fn;
}
void
upsertFileNode(fileNode *fn, char *token, int count)
{
	Occurrences *tok;

	if (fn == NULL) {
		perror("filaNode cannot be NULL in upsertFileNode\n");
		return;
	}
	if (token == NULL) {
		perror("token cannot be NULL in upsertFileNode\n");
		return;
	}
	if (count == 0) {
		perror("count cannot be 0 in upsertFileNode\n");
		return;
	}

	// pair token with count
	tok = OccurrencesFactory(token, count);
	if (tok == NULL) {
		perror("Unable to allocate space for fn->filename in fileNodeFactory\n");
		return ;
	}
	SLInsert(fn->tokens, tok);
}

// delete
void
destroyFileNode(void *f)
{
	fileNode *fn = (fileNode*)f;
	SLDestroy(fn->tokens);
	free(fn->filename);
	free(fn);
}

// compare
int
compareFileNodes(void *fa, void *fb)
{
	fileNode *fna = (fileNode*)fa;
	fileNode *fnb = (fileNode*)fb;
	return strcmp(fna->filename, fnb->filename);
}

void
printFileNode(fileNode *fn)
{
	SortedListIteratorPtr  iter;
	Occurrences           *occ;
	char                  *token;
	int                    count;

	printf("%10s : \n     { \n", fn->filename);

	iter = SLCreateIterator(fn->tokens);

	occ = SLGetItem(iter);
	while (occ != NULL) {
		token = occ->file;
		count = occ->count;
		printf("     %20s : %d,\n", token, count);
		occ = SLNextItem(iter);
	}
	printf("     } \n");
	SLDestroyIterator(iter);
}
