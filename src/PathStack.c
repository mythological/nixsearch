// THIS IS THE FIXED VERSION


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "St4x.h"
#include "PathStack.h"

#define DEFAULT_START_SIZE 100

// get the last director off the stack and remove it
void PSPop(PathStack *ss) {

	if (ss->size == 0 || ss->len == 0)
		return;

	
	int prevlen = STXPop(ss->lenstack);
	ss->len = prevlen;
	ss->arr[ss->len] = '\0';
	ss->levels--;
}

// push a directory entry onto the stack
void PSPush(char *str, PathStack *ss) {

	if (ss->len == 0) {

		int inLen = strlen(str);
		while (ss->len + inLen >= ss->size) {

			PSGrow(ss);
		}
		int oldLen = ss->len;
		int i = 0;
		int j = oldLen;
		while (i < inLen) {

			ss->arr[j++] = str[i++];
		}
		ss->len = j;
		ss->arr[j] = '\0';
		STXPush(oldLen, ss->lenstack);
		ss->levels++;
	}
	else {

		int inLen = strlen(str) + 1;
		while (ss->len + inLen >= ss->size) {

			PSGrow(ss);
		}
		int oldLen = ss->len;
		int i = 0;
		int j = oldLen;
		ss->arr[j++] = '/';
		while (i + 1 < inLen) {

			ss->arr[j++] = str[i++];
		}
		ss->len = j;
		ss->arr[j] = '\0';
		STXPush(oldLen, ss->lenstack);
		ss->levels++;
	}
}
// destroy the path stack
void PSDestroy(PathStack *ss) {

	free(ss->arr);
	free(ss->lenstack);
	free(ss);

}
// create a new PathStack
PathStack * PSCreate() {

	// allocate memory for the PathStack
	PathStack *ss = (PathStack*)malloc(sizeof(PathStack) * DEFAULT_START_SIZE);
	if ( ss == NULL )
		return NULL;

	// initalize size and len
	ss->len = 0;
	ss->size = DEFAULT_START_SIZE;
	ss->levels = 0;

	// allocate space for the string
	ss->arr = (char*)malloc(sizeof(char) * DEFAULT_START_SIZE);
	if ( ss == NULL )
		return NULL;

	// create the len stack
	ss->lenstack = STXSpawn();
	if (ss->lenstack == NULL)
		return NULL;

	// initialize string to null
	int i;
	for (i = 0; i < ss->size; ++i) {

		ss->arr[i] = ' ';
	}
	ss->arr[0] = '\0';
	
	return ss;
}
// gets path string
char *PSGet(PathStack *ss) {

	return ss->arr;
}
// increase the stack's maximum size
void PSGrow(PathStack *ss) {

	ss->size *= 2;
	char* next = (char*)malloc(sizeof(char) * ss->size);
	int i = 0;
	while ( ss->arr[i] != '\0' ) {

		next[i] = ss->arr[i];
		i++;
	}
	next[i] = '\0';
	free(ss->arr);
	ss->arr = next;
}

