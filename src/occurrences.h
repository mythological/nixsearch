#ifndef __OCC_OCC__
#define __OCC_OCC__

struct Occurrences {
	char *file;
	int count;
};
typedef struct Occurrences Occurrences;
void destroyOccurrences(void *p);
Occurrences * OccurrencesFactory(char *file, int count);
int compareOccurrences(void *p1, void *p2);

#endif
