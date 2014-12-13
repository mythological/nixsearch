#include "search_functions.h"

void
search_or(inv_index_t *index, command_t *com)
{
	SortedListPtr src = NULL;
	SortedListPtr dst = NULL; 

	if (index == NULL) {
		printf("No index filed loaded.\n");
		return;
	}
	fileNode *fn;
	char *token = nextArg(com);
	if (token == NULL) {
		free(token);
		return;
	}
	
	if (cfuhash_exists(index->hash, token)) {

		src = cfuhash_get(index->hash, token);

		dst = SLCreate(compareFileNodes, destroyFileNode);

		SortedListIteratorPtr iter = SLCreateIterator(src);
		Occurrences *occ = SLGetItem(iter);
		while (occ) {
			fn = fileNodeFactory(occ->file, token, occ->count);
			SLInsert(dst, fn);
			occ = SLNextItem(iter);
		}
		SLDestroyIterator(iter);
	}
	free(token);

	token = nextArg(com);
	while (token != NULL) {
		printf("%s\n", token);
		if (cfuhash_exists(index->hash, token)) {
			src = cfuhash_get(index->hash, token);
			SortedListPtr dstN = SLCreate(compareFileNodes, destroyFileNode);
			
			SortedListIteratorPtr iter = SLCreateIterator(src);
			Occurrences *occ = SLGetItem(iter);
			while(occ) {
				fn = fileNodeFactory(occ->file, token, occ->count);
				SLInsert(dstN, fn);
				occ = SLNextItem(iter);
			}
			SLDestroyIterator(iter);
			if (dst == NULL) {
				dst = dstN;
			}
			else {
				dst = merge_or(dst, dstN);
			}
		}

		free(token);
		token = nextArg(com);
	}
	if (dst == NULL) {
		printf("{}\n");
		return;
	}
	print_results(dst);
	SLDestroy(dst);
}

void
search_and(inv_index_t *index, command_t *com)
{
	if (index == NULL) {
		printf("No index filed loaded.\n");
		return;
	}
	fileNode *fn;
	char *token = nextArg(com);
	if (token == NULL) {
		free(token);
		return;
	}
	
	if (!cfuhash_exists(index->hash, token)) {
		printf("{}\n");
		free(token);
		return;
	}
	SortedListPtr src = cfuhash_get(index->hash, token);

	SortedListPtr dst = SLCreate(compareFileNodes, destroyFileNode);

	SortedListIteratorPtr iter = SLCreateIterator(src);
	Occurrences *occ = SLGetItem(iter);
	while (occ) {
		
		fn = fileNodeFactory(occ->file, token, occ->count);
		SLInsert(dst, fn);
		occ = SLNextItem(iter);
	}
	SLDestroyIterator(iter);
	free(token);

	token = nextArg(com);
	while (token != NULL) {
		if (cfuhash_exists(index->hash, token) == 0) {
			printf("{}\n");
			SLDestroy(src);
			SLDestroy(dst);
			free(token);
			return;
		}
		src = cfuhash_get(index->hash, token);
		SortedListPtr dstN = SLCreate(compareFileNodes, destroyFileNode);
		
		iter = SLCreateIterator(src);
		occ = SLGetItem(iter);
		while(occ) {
			fn = fileNodeFactory(occ->file, token, occ->count);
			SLInsert(dstN, fn);
			occ = SLNextItem(iter);
		}
		SLDestroyIterator(iter);
		dst = merge_and(dst, dstN);

		free(token);
		token = nextArg(com);
	}
	print_results(dst);
	SLDestroy(dst);
}

void
display_help(command_t *com)
{
		if (com == NULL) {
			return;
		}
		printf("\n");
		char *whatami = nextArg(com);
		if (whatami == NULL) {
			printf("No command specified.\n\n");
			return;
		}

		if ( !(strcmp(whatami, "q")   ) ||
				  !(strcmp(whatami, "quit"))   ) {
			printf("Usage: %s\n", whatami);
			printf("Description: Exit the program\n");
		}
		else if ( !(strcmp(whatami, "sa")) ) {
			printf("Usage: %s <token 1> <token 2> . . . <token n>\n", whatami);
			printf("Description: ");
			printf("List files that contain all tokens specified.\n");
		}	
		else if ( !(strcmp(whatami, "so")) ) {
			printf("Usage: %s <token 1> <token 2> . . . <token n>\n", whatami); printf("Description: ");
			printf("List files that contain any of the tokens specified.\n");
		}	
		else if ( !(strcmp(whatami, "cls")   ) ||
				  !(strcmp(whatami, "clear"))   ) {
			printf("Usage: %s\n", whatami);
			printf("Description: clear the screen.\n");
		}	
		else if ( !(strcmp(whatami, "ls")) ) {
			printf("Usage: %s <flags> \n", whatami);
			printf("Description: Call ls command to show contents of current\n");
			printf("working directory. See 'man ls' for more details.\n");
		}	
		else if ( !(strcmp(whatami, "cd")) ) {
			printf("Usage: %s <directory name>\n", whatami);
			printf("Description: Change directory to <directory name>\n");
		}	
		else if ( !(strcmp(whatami, "l")   ) ||
				  !(strcmp(whatami, "load"))   ) {
			printf("Usage: %s <inverted index file>\n", whatami);
			printf("Description: load index file to search.\n");
		}	
		else if ( !(strcmp(whatami, "h")   ) ||
				  !(strcmp(whatami, "help"))   ) {
			printf("Usage: %s <command>\n", whatami);
			printf("User needs help using help. The meta is getting out of ");
			printf("hand.\n");
		}
		else if ( !(strcmp(whatami, "c")   ) ||
				  !(strcmp(whatami, "commands"))   ) {
			printf("Usage: %s\n", whatami);
			printf("Display list of available commands.\n");
		}	
		else if ( !(strcmp(whatami, "i")   ) ||
				  !(strcmp(whatami, "index"))   ) {
			printf("Usage: %s <inverted index file> <input file or dir>\n", whatami);
			printf("Create inverted index file from <input file or dir>.\n");
		}	
		else {
			printf("Type 'commands' or 'c' for a list of available commands.\n");
			printf("Type 'quit' or 'q' to quit.\n");
		}
		free(whatami);
		printf("\n");
}

void
display_commands(void)
{
	printf("\nListing available commands\n\n");
	printf("| Command  | Short vers. |\n");
	printf("+----------+-------------+\n");
	printf("| quit     | q           |\n");
	printf("| sa       |             |\n");
	printf("| so       |             |\n");
	printf("| clear    | cls         |\n");
	printf("| ls       |             |\n");
	printf("| cd       |             |\n");
	printf("| load     | l           |\n");
	printf("| help     | h           |\n");
	printf("| index    | i           |\n");
	printf("| commands | c           |\n\n");
	// print usage
}

SortedListPtr 
merge_or(SortedListPtr sla, SortedListPtr slb)
{
	SortedListPtr		     sl;
	SortedListIteratorPtr iterA;
	SortedListIteratorPtr iterB;
	Occurrences    *tokA, *tokB;
	int                  result;
	fileNode*                fn;
	fileNode*               fna;
	fileNode*               fnb;

	sl    = SLCreate(compareFileNodes, destroyFileNode);
	iterA = SLCreateIterator(sla);
	iterB = SLCreateIterator(slb);

	
	fna    = SLGetItem(iterA);
	fnb    = SLGetItem(iterB);
	while (fna && fnb) {
		
		result = compareFileNodes(fna, fnb);

		if (result == 0) {
			SortedListIteratorPtr iter_tokens_a = SLCreateIterator(fna->tokens);
			SortedListIteratorPtr iter_tokens_b = SLCreateIterator(fnb->tokens);
			
			tokA = SLGetItem(iter_tokens_a);	
			fn = fileNodeFactory(fna->filename, tokA->file, tokA->count);

			tokA = SLNextItem(iter_tokens_a);
			while (tokA) {

				upsertFileNode(fn, tokA->file, tokA->count);
				tokA = SLNextItem(iter_tokens_a);
			}
			tokB = SLGetItem(iter_tokens_b);
			while (tokB) {

				upsertFileNode(fn, tokB->file, tokB->count);
				tokB = SLNextItem(iter_tokens_b);
			}
			SLInsert(sl, fn);

			SLDestroyIterator(iter_tokens_a);
			SLDestroyIterator(iter_tokens_b);

			fnb = SLNextItem(iterB);
			fna = SLNextItem(iterA);
		}
		else if (result < 0) {
			SortedListIteratorPtr iter_tokens_b = SLCreateIterator(fnb->tokens);
			tokB = SLGetItem(iter_tokens_b);	
			fn = fileNodeFactory(fnb->filename, tokB->file, tokB->count);

			tokB = SLNextItem(iter_tokens_b);
			while (tokB) {

				upsertFileNode(fn, tokB->file, tokB->count);
				tokB = SLNextItem(iter_tokens_b);
			}
			SLInsert(sl, fn);
			SLDestroyIterator(iter_tokens_b);

			fnb = SLNextItem(iterB);
		}
		else {
			SortedListIteratorPtr iter_tokens_a = SLCreateIterator(fna->tokens);
			tokA = SLGetItem(iter_tokens_a);	
			fn = fileNodeFactory(fna->filename, tokA->file, tokA->count);

			tokA = SLNextItem(iter_tokens_a);
			while (tokA) {

				upsertFileNode(fn, tokA->file, tokA->count);
				tokA = SLNextItem(iter_tokens_a);
			}
			SLInsert(sl, fn);
			SLDestroyIterator(iter_tokens_a);

			fna = SLNextItem(iterA);
		}
	}
	while (fna) {
		SortedListIteratorPtr iter_tokens_a = SLCreateIterator(fna->tokens);
		tokA = SLGetItem(iter_tokens_a);	
		fn = fileNodeFactory(fna->filename, tokA->file, tokA->count);

		tokA = SLNextItem(iter_tokens_a);
		while (tokA) {

			upsertFileNode(fn, tokA->file, tokA->count);
			tokA = SLNextItem(iter_tokens_a);
		}
		SLInsert(sl, fn);
		SLDestroyIterator(iter_tokens_a);

		fna = SLNextItem(iterA);
	}
	while (fnb) {
		SortedListIteratorPtr iter_tokens_b = SLCreateIterator(fnb->tokens);
		tokB = SLGetItem(iter_tokens_b);	
		fn = fileNodeFactory(fnb->filename, tokB->file, tokB->count);

		tokB = SLNextItem(iter_tokens_b);
		while (tokB) {

			upsertFileNode(fn, tokB->file, tokB->count);
			tokB = SLNextItem(iter_tokens_b);
		}
		SLInsert(sl, fn);
		SLDestroyIterator(iter_tokens_b);

		fnb = SLNextItem(iterA);
	}

	SLDestroyIterator(iterA);
	SLDestroyIterator(iterB);
	SLDestroy(sla);
	SLDestroy(slb);
	return sl;
}

SortedListPtr 
merge_and(SortedListPtr sla, SortedListPtr slb)
{
	SortedListPtr		     sl;
	SortedListIteratorPtr iterA;
	SortedListIteratorPtr iterB;
	Occurrences    *tokA, *tokB;
	int                  result;
	fileNode*                fn;
	fileNode*               fna;
	fileNode*               fnb;

	sl    = SLCreate(compareFileNodes, destroyFileNode);
	iterA = SLCreateIterator(sla);
	iterB = SLCreateIterator(slb);

	
	fna    = SLGetItem(iterA);
	fnb    = SLGetItem(iterB);
	while (fna && fnb) {
		
		result = compareFileNodes(fna, fnb);

		if (result == 0) {
			SortedListIteratorPtr iter_tokens_a = SLCreateIterator(fna->tokens);
			SortedListIteratorPtr iter_tokens_b = SLCreateIterator(fnb->tokens);
			
			tokA = SLGetItem(iter_tokens_a);	
			fn = fileNodeFactory(fna->filename, tokA->file, tokA->count);

			tokA = SLNextItem(iter_tokens_a);
			while (tokA) {

				upsertFileNode(fn, tokA->file, tokA->count);
				tokA = SLNextItem(iter_tokens_a);
			}
			tokB = SLGetItem(iter_tokens_b);
			while (tokB) {

				upsertFileNode(fn, tokB->file, tokB->count);
				tokB = SLNextItem(iter_tokens_b);
			}
			SLInsert(sl, fn);

			SLDestroyIterator(iter_tokens_a);
			SLDestroyIterator(iter_tokens_b);

			fnb = SLNextItem(iterB);
			fna = SLNextItem(iterA);
		}
		else if (result < 0) {
			fnb = SLNextItem(iterB);
		}
		else {
			fna = SLNextItem(iterA);
		}
	}
	SLDestroyIterator(iterA);
	SLDestroyIterator(iterB);
	SLDestroy(sla);
	SLDestroy(slb);
	return sl;
}

void
print_results(SortedListPtr sl)
{
	printf("{\n");
	SortedListIteratorPtr iter = SLCreateIterator(sl);	
	fileNode *fn = SLGetItem(iter);	
	while (fn) {
		printFileNode(fn);
		fn = SLNextItem(iter);
	}
	SLDestroyIterator(iter);
	printf("}\n");
}

void
changeDir(command_t *com)
{
	char *dir = nextArg(com);
	if (dir == NULL) {
		chdir(getenv("HOME"));
	}
	else {
		chdir(dir);
	}
	free(dir);
}
inv_index_t *
newIndex(command_t *com, inv_index_t *index)
{
	char *filename;
	inv_index_t *next;

	filename = nextArg(com);
	if (filename == NULL) {
		printf("No index file specified.\n");
		return index;
	}
	next = load_index(filename);
	if (index != NULL) {
		destroy_index(index);
	}
	free(filename);
	return next;
}

inv_index_t *
use_indexer(command_t *com, inv_index_t *index)
{
	char *outputFile = NULL;
	char *inputFile  = NULL;

	inputFile = nextArg(com);
	if (inputFile == NULL) {
		printf("No output file specified.\n");
		return index;
	}

	outputFile = nextArg(com);
	if (outputFile == NULL) {
		printf("No input file specified.\n");
		free(inputFile);
		return index;
	}

	printf("outputfile: %s\n", outputFile);
	printf("inputfile: %s\n", inputFile);
	
	if (run_indexer(outputFile, inputFile) == 0) {
		free(outputFile);	
		free(inputFile);	
		return index;
	}
	free(outputFile);	
	free(inputFile);	

	resetIter(com);
	free(nextArg(com));
	return newIndex(com, index);
}
