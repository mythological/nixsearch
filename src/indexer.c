#include "indexer.h"

#ifndef ALPHALEN
#define ALPHALEN 36
#endif

TrieT base;

// creates a prefix tree Node
Node*
nodeFactory()
{
	// instantiate new Node
	Node* next = (Node*)malloc(sizeof(Node));
	if (next == NULL) {

		fprintf(stderr, "in %s on line %d\n", __FILE__, __LINE__);
		exit(1);
	}

	int i = 0;

	// initialize children
	for(i = 0; i < ALPHALEN; ++i)
		next->children[i] = NULL;

	// initialize attributes	
	next->isDictWord = 0;
	next->superwords = 0;
	next->matches = 0;
	next->sl = SLCreate(compareOccurrences, destroyOccurrences);

	return next;
}


// creates the prefix tree
void initialize() { base.root = nodeFactory(); }

// recursive auxiliary to the destroy() method
void
destroyHelper(Node* n)
{
	// do not accept null pointers
	if (n == NULL)
		return;

	// traverse to each child of the current node, then free them
	int i;
	for(i = 0; i < ALPHALEN; ++i) {

		if (n->children[i] != NULL) {
	
			destroyHelper(n->children[i]);
			SLDestroy(n->children[i]->sl);
			free(n->children[i]);
		}
	}
	return;
}
void
destroy()
{
	destroyHelper(base.root);
	SLDestroy(base.root->sl);
	free(base.root);
}



// maps a char to its lexographical position in an alphanumeric alphabet
int
ascii2index(char c)
{
	if (c >= 'a' && c <= 'z')
		return (int)c - 'a';
	else if (c >= 'A' && c <= 'Z')
		return (int)c - 'A';
	else if (c >= '0' && c <= '9')
		return (int)c - '0' + 26;

	return -1;
}

// maps a lexicographical position to its associated ascii code
char
index2ascii(int index)
{
	if (index <= 25)
		return 'a' + (char)index;

	return index + '0' - 26;
}

// given a dict_file and a current
int
TrieInsert(FILE *dict_file, Node *current)
{
	// get the next char in the file
	char c = fgetc(dict_file);

	// if we've reached the end if the file,
	// we return false to indicate that we should
	// break out of the while look in readDict()
	if (c == EOF)
		return 0;

	// attempt to convert c into an array index
	int index = ascii2index(c);

	// if i is negative 1, then c isn't a letter.
	// we return i to indicate that we should
	// keep reading from the file
	if ( index < 0 )
		return -1;

	// othewise we either insert a new node or update the current node
	if (current->children[index] == NULL) 
		current->children[index] = nodeFactory(c);

	int leaf = TrieInsert(dict_file, current->children[index]);
	if ( leaf == -1 ) {
		current->children[index]->isDictWord = 1;
		leaf = 1;
	}

	return leaf;
}

void
readDict(FILE *dict_file, char* filename)
{
	base.file = filename;
	while ( TrieInsert(dict_file, base.root) );

	rewind(dict_file);
}

void
addWord(char* word)
{
	/// return if word is null, ends in a null terminator, or trie is empty
	if (word == NULL || *word == '\0' || base.root->children == NULL)
		return;

	// create a word iterator
	char *iter = word;
	Node *current = base.root;

	// if current is null, then the word isn't in the tree in the first place
	if (current == NULL)
		return;

	// iterate through the word until we reach the last character
	int i = 0;
	while (iter[i] != '\0' && current->children[ascii2index(iter[i])] != NULL) {

		current->superwords++;
		current = current->children[ascii2index(iter[i])];
		i++;
	}
	if (iter[i] == '\0')
		current->matches++;
	else
		current->superwords++;
}
void
scanData(FILE *datHandle)
{
	if (datHandle == NULL)
		return;

	// create a char stack to hold the letters
	St4x *s = STXSpawn();

	// enter indefinite loop
	while (1) {

		// clear preceeding whitespace and nonletter chars
		char c = fgetc(datHandle);
		while (c != EOF && ascii2index(c) < 0)
			c = fgetc(datHandle);

		// if we've reached the end of the file, break out of the loop
		if (c == EOF)
			break;

		// while letters are alphanumeric and not EOF, push them onto the stack
		while (ascii2index(c) >= 0 && c != EOF) {

			STXPush(c, s);
			c = fgetc(datHandle);
		}
		// add the word to the trie
		int len = STXLen(s);
		char* word = (char*)malloc(sizeof(char) * len + 1);
		if (word == NULL) {
			fprintf(stderr, "In %s on line %d:\n", __FILE__, __LINE__);
			perror("Malloc faled.\n");
			exit(1);
		}
		word[len] = '\0';
		int i;
		for (i = len-1; i >= 0; --i) {

			word[i] = STXPop(s);
		}
		addWord(word);

		// free the word
		free(word);
	}
		
	// destroy the stack	
	STXDestroy(s);

	return;
}

// traverses through trie using tail recursion to efficiently reset
// the nodes for next file
int
resetDataHelper(Node *current, St4x *s, char c, int prefixes)
{

	if (current != NULL) {

		STXPush(c, s);

		current->matches = 0;
		current->superwords = 0;

		int i;
		for(i = 0; i < ALPHALEN; ++i) {
		
			resetDataHelper(current->children[i], s, index2ascii(i),
									 current->matches + prefixes);
		}
		STXPop(s);
	}
	return 0;
}

// resets the trie for use with next file by calling resetDataHelper on each
// child of the root node
void
resetData()
{
	
	St4x *s = STXSpawn();
	if (s == NULL) {

		fprintf(stderr, "%s on line %d\n",__FILE__,__LINE__ - 3);
		return;
	}
	int i;
	for(i = 0; i < ALPHALEN; ++i) {
	
		resetDataHelper(base.root->children[i], s, index2ascii(i), 0);
	}
	STXDestroy(s);
}

// efficiently traverses the trie using tail recursion to store the indexing data
// at every node
int
storeDataHelper(Node *current, St4x *s, char c, int prefixes)
{
	if (current != NULL) {

		STXPush(c, s);

		// if we've reached the end of the token, update the token's SortedList
		if (current->isDictWord == 1 && current->matches > 0) {
			Occurrences *oc = OccurrencesFactory(base.file, current->matches);
			SLInsert(current->sl, oc);
		}

		// traverse to each of the node's children
		int i;	
		for(i = 0; i < ALPHALEN; ++i) {
		
			storeDataHelper(current->children[i], s, index2ascii(i),
									 current->matches + prefixes);
		}
		STXPop(s);
	}
	return 0;
}

// updates the SortedLists at each of the nodes in the prefix tree
void
storeData()
{
	// create a character stack
	St4x *s = STXSpawn();
	if (s == NULL) {

		fprintf(stderr, "%s on line %d\n",__FILE__,__LINE__ - 3);
		return;
	}
	// begin to traverse the prefix tree
	int i;
	for(i = 0; i < ALPHALEN; ++i) {
	
		storeDataHelper(base.root->children[i], s, index2ascii(i), 0);
	}
	STXDestroy(s);
}

// uses tail recursion to efficiently print the entire trie to the file
int
printAllDataHelper(Node *current, St4x *s, char c, int prefixes, FILE *f)
{
	if (current != NULL) {

		// current char onto the stack
		STXPush(c, s);

		// check to see if we've reached the end of a token
		if (current->isDictWord == 1) {

			// create an iterator
			SortedListIteratorPtr iter = SLCreateIterator(current->sl);

			// copy the current token into a string
			int len = STXLen(s);
			char* word = (char*)malloc(sizeof(char) * len + 1);
			if (word == NULL) {
				fprintf(stderr, "In %s on line %d:\n", __FILE__, __LINE__);
				perror("Malloc faled.\n");
				exit(1);
			}
			word[len] = '\0';
			int i;
			for (i = len-1; i >= 0; --i) {

				word[i] = STXPop(s);
			}
			i = 0;
			while (word[i] != '\0')
				STXPush(word[i++], s);

			// print the token into the output file, along with
			// its list of occurrences
			fprintf(f, "<list> %s\n", word);
			free(word);
			Occurrences *oc = (Occurrences*)SLGetItem(iter);
			int ii = 0;
			while (oc->file[ii] != '\0') {
				if (oc->file[ii] == ' ') {
					oc->file[ii] = '#';
				}
				ii++;	
			}
			fprintf(f, "%s %d", oc->file, oc->count);
			int m = 1;
			while ( (oc = SLNextItem(iter)) ) {
				if (m % 5 == 0) {
					fprintf(f,"\n");
				}
				int i = 0;
				while (oc->file[i] != '\0') {
					if (oc->file[i] == ' ') {
						oc->file[i] = '#';
					}
					i++;	
				}
				fprintf(f, " %s %d", oc->file, oc->count);
			}
			fprintf(f, "\n</list>\n");

			// destroy the iterator
			SLDestroyIterator(iter);
		}

		// traverse to each of the current node's children
		int i;
		for(i = 0; i < ALPHALEN; ++i) {
		
			printAllDataHelper(current->children[i], s, index2ascii(i),
									 current->matches + prefixes, f);
		}

		// remove the current char from the stack
		STXPop(s);
	}
	return 0;
}
// prints the prefix tree in lexicographical order
void
printAllData(char* outputFile)
{
	// create a character stack
	St4x *s = STXSpawn();
	if (s == NULL) {

		fprintf(stderr, "%s on line %d\n",__FILE__,__LINE__ - 3);
		return;
	}
	// open the output file as a file stream
	FILE *oHandle = fopen(outputFile,"w");
	if (oHandle == NULL) {
		perror("Unable to open the output file.\n");
		exit(1);
	}
	// begin to traverse the tree
	int i;
	for(i = 0; i < ALPHALEN; ++i) {
	
		printAllDataHelper(base.root->children[i], s, index2ascii(i),0,oHandle);
	}
	// close the file and destroy the stack
	fclose(oHandle);
	STXDestroy(s);
}
