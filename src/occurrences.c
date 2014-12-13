#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "occurrences.h"


// destroys an Occurrences struct
void 
destroyOccurrences(void *p)
{
	Occurrences *oc = (Occurrences*)p;
	free(oc->file);
	free(oc);
}
// creates an Occurrences struct with a particular file name and count
Occurrences *
OccurrencesFactory(char *file, int count)
{
	Occurrences *oc = (Occurrences*)malloc(sizeof(Occurrences));
	int len = strlen(file);
	int i = 0;
	oc->file = (char*)malloc(sizeof(char) * len + 1);
	while ( *(file + i) != '\0' ) {
		oc->file[i] = file[i];
		i++;
	}
	oc->file[i] = '\0';
	oc->count = count;
	return oc;
}

// compares two Occurrences struct
int
compareOccurrences(void *p1, void *p2)
{
	Occurrences *o1 = p1;
	Occurrences *o2 = p2;
	return o1->count - o2->count;
}
