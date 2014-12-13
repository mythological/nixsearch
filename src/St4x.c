#include <stdio.h>
#include <stdlib.h>
#include "St4x.h"

// a single node in the stack
struct SNode {

	struct SNode *prev;
	int c;
};
typedef struct SNode SNode;

// stack structure
struct St4x {
	
	SNode *top;
	int size;
};

// remove last item from stack and return it
int STXPop(St4x *s) {

	if (s == NULL) {
		return 0x00;
	}

	if (s->top == NULL) {

		return 0x00;
	}

	int c = s->top->c;	
	SNode *tmp = s->top;
	s->top = s->top->prev;
	free(tmp);
	s->size--;
	return c;
}

// destroy the stack
void STXDestroy(St4x *s) {

	if (s == NULL) {

		return;
	}

	while(s->size > 0) {
		
		STXPop(s);
	}
	free(s);
}

// assists with printing the stack
static void Gutenb0rg(SNode *current) {

	if (current == NULL) {

		return;
	}

	Gutenb0rg(current->prev);
	printf("%c", (char)current->c);
}
// prints the contents of the stack
void STXPrint(St4x *s) {

	if (s == NULL) {
		return;
	}
	Gutenb0rg(s->top);
}

// push onto the stack
int STXPush(int c, St4x *s) {

	if (s == NULL) {

		return 0;
	}
	
	
	SNode *n = (SNode*)malloc(sizeof(SNode));
	if (n == NULL) {

		return 0;
	}

	n->c = c;
	n->prev = NULL;
	
	if (s->size == 0) {

		s->top = n;
	}
	else {

		n->prev = s->top;
		s->top = n;
	}

	s->size++;
	return 1;
}

// create a new stack
St4x *STXSpawn() {

	St4x *s = (St4x*)malloc(sizeof(St4x));
	if (s == NULL) {

		return 0;
	}

	s->top = NULL;
	s->size = 0;
	return s;
}

// get the size of the stack
int  STXLen(St4x *s) {

	return s->size;
}

