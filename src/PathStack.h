#include <stdio.h>
#include "St4x.h"

#ifndef __STACK__STRING__
#define __STACK__STRING__

struct PathStack {
	char *arr;
	St4x *lenstack; // a stack of the previous len values
	int len; // logical length of the string
	size_t size; // physical length of the string
	int levels;
};
typedef struct PathStack PathStack;

void PSGrow(PathStack *path);
char *PSGet(PathStack *path);
void PSPop(PathStack *path);
PathStack * PSCreate();
void PSDestroy(PathStack *path);
void PSPush(char *str, PathStack *path);


#endif
