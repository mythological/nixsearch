
#ifndef __INDEX_TRIE__
#define __INDEX_TRIE__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "St4x.h"
#include "sorted-list.h"
#include "occurrences.h"


struct Node {

	struct Node* children[36];
	int isDictWord;
	int superwords;
	int matches;
	SortedListPtr sl;
};
typedef struct Node Node;
struct TrieT {
	Node* root;	
	char* file;
};
typedef struct TrieT TrieT;


void initialize();

void destroy();


// maps a char to its lexographical position in an alphanumeric alphabet
int ascii2index(char c);
char index2ascii(int index);


int TrieInsert(FILE *dict_file, Node *current);

void readDict(FILE *dict_file, char* filename);
void addWord(char* word);
void scanData(FILE *datHandle);
int resetDataHelper(Node *current, St4x *s, char c, int prefixes);
void resetData();
int storeDataHelper(Node *current, St4x *s, char c, int prefixes);
void storeData();
int printAllDataHelper(Node *current, St4x *s, char c, int prefixes, FILE *f);
void printAllData(char* outputFile);
#endif
