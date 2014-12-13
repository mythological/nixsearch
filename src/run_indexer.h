#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include "PathStack.h"
#include "indexer.h"


struct Config {

	char *basedir;
	char *outputFile;
	int flag_fileonly;
	int flag_errmsg;
};
typedef struct Config Config;

Config *setup(char *outputFile, char *inputFile);
char*  prepareFilename(char* filename);
int    validateInputs(char *inputFile, char *outputFile, Config *config);
int    run_indexer(char *outputFile, char *inputFile);

//void dirTraverser(Config *config);
void dirTraverser(PathStack*, Config*);
void fileParser(PathStack *path, Config *config);

void endOfJerb(Config *config);
