#include "inv_index_t.h"

static void
free_data(void *data)
{
	SortedListPtr sl = (SortedListPtr)data;
	SLDestroy(sl);
}

inv_index_t*
load_index(char *filename)
{
	FILE  *inHandle  = NULL;
	size_t linecount = 0;
	char   c         = ' ';
	char  *line      = NULL;
	char  *tok   	 = NULL;
	char  *key   	 = NULL;
	size_t len  	 = 128;
	SortedListPtr sl = NULL;

	if (validateIndex(filename) == 0) {
		return NULL;
	}

	// open the inverted index file	
	inHandle = fopen(filename, "r");
	if (inHandle == NULL) {
		//printf("Unable to load index file: %s\n", filename);
		return NULL;
	}

	
	// get the number of lines in the file
	while( c = fgetc(inHandle), linecount += 1 * (c == '\n'), c != EOF );
	
	// allocate space for the inverted index
	inv_index_t *index = (inv_index_t*)malloc(sizeof(inv_index_t));
	if (index == NULL) {

		printf("Unable to allocate space for inverted index.\n");
		return NULL;
	}

	index->index_file = (char*)malloc(sizeof(char) * strlen(filename) + 1);
	strcpy(index->index_file, filename);

	// initialize a new hash table
	cfuhash_table_t *hash = cfuhash_new_with_initial_size(linecount);
	if (hash == NULL) {

		perror("Unable to allocate space for index->hash\n");
		return NULL;
	}

	// set line count back to one, rewind the file position indicator
	linecount   = 1;
	rewind(inHandle);

	// parse the inverted index file
	while (getline(&line, &len, inHandle) != -1) {
		tok = strtok(line, " \n\t");

		// if we've reached an opening list tag, create a new sorted list
		if (strncmp(tok, "<list>", (size_t)6) == 0) {
			key = strdup(strtok(NULL, " \n\t"));
			if (key != NULL) {
				sl = SLCreate(compareOccurrences, destroyOccurrences);
			}
		}
		// otherwise if we've reached a closing list tag, store the list
		else if (strncmp(tok, "</list>", (size_t)7) == 0) {
			
			// insert sorted list into hash table
			cfuhash_put(hash, key, sl);
			free(key);
		}
		// otherwise read { token : count } pairs from line, unless list was
		// failed to initialize
		else if (sl != NULL) {
			char *file       = NULL;
			char *count      = NULL;
			char *sp         = NULL;
			Occurrences *occ = NULL;

			file = tok;
			sp = file;
			while (*sp != '\0') {
				if (*sp == '#') {
					*sp = ' ';
				}
				sp++;
			}
			count = strtok(NULL, " \n\t");

			occ = OccurrencesFactory(file, atoi(count));
			SLInsert(sl, occ);


			while (file != NULL) {

				file = strtok(NULL, " \r\n\t");
				if (file == NULL) {
					// end of line reached
					break;
				}
				sp = file;
				while (*sp != '\0') {
					if (*sp == '#') {
						*sp = ' ';
					}
					sp++;
				}
				count = strtok(NULL, " \n\r\t");
				if (count == NULL || atoi(count) == 0) {
					fprintf(stderr, "Malformed line: %zu\n", linecount);
					break;
				}

				occ = OccurrencesFactory(file, atoi(count));

				SLInsert(sl, occ);
			}

		}
		// if list failed to initailize, continue
		else {
			perror("List not initialized\n");
			continue;
		}
		linecount++;
	}
	// free remaining token and close the file stream
	free(tok);
	fclose(inHandle);

	// return the index file
	index->hash = hash;	
	return index;
}

void
destroy_index(inv_index_t *index)
{
	cfuhash_destroy_with_free_fn(index->hash, free_data);
	free(index->index_file);
	free(index);
}

int
validateIndex(char *inputFile)
{
	struct stat statbuf;

	// run stat on the input source
	if ( stat(inputFile, &statbuf) == -1) {

		if (ENOENT == errno) {

			fprintf(stderr, "Error: %s does not exist\n", inputFile);
		}
		else {
			fprintf(stderr, "Error: Unable to stat %s\n", inputFile);
		}
		return 0;
	}

	// regular files ok. anything else is not
	if ( !(S_ISREG(statbuf.st_mode) ) ) {
		printf("That's an interesting index file you've got there. Aborting.\n");
		return 0;
	}

	// make sure we have write access to working directory
	if ( access(".", F_OK|X_OK|R_OK|W_OK) == -1 ) {

		printf("Error: Need write access to working directory to proceed.\n");
		return 0;
	}

	return 1;
}
