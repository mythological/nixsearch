#include "indexer.h"
#include "run_indexer.h"

int
run_indexer(char *outputFile, char *inputFile)
{
	printf("\n\n");
	printf("outputFile in ri: %s\n", outputFile);
	printf("inputFile in ri: %s\n", inputFile);
	printf("\n\n");

	// set things up
	Config *config = setup(outputFile, inputFile);
	if (config == NULL) {
		printf("Indexer setup failed.\n");
		return 0;
	}

	// create a PathStack
	PathStack* path = PSCreate();
	if (path == NULL) {
		perror("Fatal error.\n");
		perror("Pathstack failed. Aborting.\n");
		exit(1);
	}
	// push the basedir to path
	PSPush(config->basedir, path);

	printf("Populating index. . . \n");

	// call fileParser if we're only dealing with files,
	// dirTraverser if we have a directory
	if (config->flag_fileonly)
		fileParser(path, config);
	else
		dirTraverser(path, config);

	// remove the last entry from the path and destroy
	PSPop(path);
	PSDestroy(path);

	// print report, tear things down, and quit
	endOfJerb(config);

	return 1;
}

Config *
setup(char *outputFile, char *inputFile)
{

	inputFile = prepareFilename(inputFile);
	outputFile = prepareFilename(outputFile);

	if (inputFile == NULL || outputFile == NULL) {
		if (inputFile != NULL) {
			free(inputFile);
		}
		if (outputFile != NULL) {
			free(outputFile);
		}
		return NULL;
	}

	// allocate space for config
	Config *config = (Config*)malloc(sizeof(Config));
	if (config == NULL) {
		fprintf(stderr, "Malloc failed: %s on line %d", __FILE__, __LINE__-2);
		exit(1);
	}

	if (validateInputs(inputFile, outputFile, config) == 0) {
		free(config);
		free(outputFile);
		free(inputFile);
		return NULL;
	}

	// set configs
	config->basedir = inputFile;
	config->outputFile = outputFile;
	config->flag_errmsg = 0;

	printf("Initializing. . . \n");
	initialize();	
	return config;
}
void
dirTraverser(PathStack* path, Config *config)
{
	// declare locals
	char   *dirname;
	DIR    *dir;
	struct dirent *entry;
	struct stat statbuf;
	int    flag_isdir = 0;
	char   *name;
	
	// sanity check for troll args
	if (path == NULL) {

		return;
	}

	// get the directory name
	dirname = PSGet(path);

	//  open the directory
	dir = opendir(dirname);
	if (dir == NULL)
		return;

	// enter indefinite loop
	while (1) {
	
		// get the next entry from the directory	
		entry = readdir(dir);

		// if no entries left, then bail
		if (entry == NULL)
			break;

		// ignore . and ..
		if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
			continue;

		// append direct name to path
		PSPush(entry->d_name, path);
		name = PSGet(path);
		
		
		// run stat on the entry to get file type
		if ( stat(name, &statbuf) == -1) {
	
			fprintf(stderr, "Error: Unable to stat %s\n", name);
			config->flag_errmsg = 1;
			PSPop(path);
			continue;
		}

		// regular files and directories are ok. anything else is not
		if ( S_ISREG(statbuf.st_mode) )
			flag_isdir = 0;
		else if(S_ISDIR(statbuf.st_mode))
			flag_isdir = 1;
		else {

			fprintf(stderr, "File invalid: %s\n", name);
			config->flag_errmsg = 1;
			PSPop(path);
			continue;
		}
	
		// if dirent is a directory, make sure we have read and execute privs
		if ( flag_isdir && access(name, F_OK|X_OK|R_OK) == -1 ) {

			fprintf(stderr, "Bad permissions on %s - skipping\n", name);
			config->flag_errmsg = 1;
			PSPop(path);
			continue;
		}

		// if dirent is regular file, make sure we have read privs
		if ( !flag_isdir && access(name, F_OK|R_OK) == -1 ) {

			fprintf(stderr, "Bad permissions on %s - skipping\n", name);
			config->flag_errmsg = 1;
			PSPop(path);
			continue;
		}

		// call dirTraverse if directory, fileParser if file
		if (flag_isdir)
			dirTraverser(path, config);
		else
			fileParser(path, config);

		// remove entry from path
		PSPop(path);
	}
	closedir(dir);
}

void
fileParser(PathStack *path, Config *config)
{

	// convert path to string
	char *name = PSGet(path);
	//printf("%s\n", name);

	// open the file	
	FILE* inFile = fopen(name, "r");
	if (inFile == NULL) {
		printf("Unable to open %s\n", name);
		return;
	}
	
	// forage fresh datums from file then close
	readDict(inFile, name);
	scanData(inFile);
	storeData();
	resetData();
	fclose(inFile);
}

void
endOfJerb(Config *config)
{
	// print beautiful data
	printf("Writing results to file. . . \n");
	printAllData(config->outputFile);

	// destroy the trie
	destroy();	

	// freedumz	
	free(config->outputFile);
	free(config->basedir);
	free(config);

	printf("Success!\n");
}


char*
prepareFilename(char* filename)
{
	char *newFile;
	size_t len;
	
	if (filename == NULL) {
		perror("Refusing to operate on null pointers\n");
		return NULL;
	}
	
	len = strlen(filename);
	if (len == 0) {
		printf("Refusing to operate on empty strings\n");
		return NULL;
	}

	// if the filename begins with a dash, then we must prepend './'
	// to the to avoid collosal failures
	if (filename[0] == '-') {

		newFile = (char*)malloc(sizeof(char) * len + 3);
		if (newFile == NULL) {
			fprintf(stderr, "Malloc failed: %s on line %d", __FILE__, __LINE__-2);
			exit(1);
		}

		strcpy(newFile, "./");
		strcat(newFile, filename);
		return newFile;
	}

	// fopen does not recognize ~/ as the user's home directory.
	// if file begans with ~/, we must expand it ourselves
	if (filename[0] == '~' ) {

		size_t homedirlen = strlen(getenv("HOME"));
		len = sizeof(char) * len + sizeof(char) * homedirlen;

		newFile = (char*)malloc(len);
		if (newFile == NULL) {
			fprintf(stderr, "Malloc failed: %s on line %d", __FILE__, __LINE__-2);
			exit(1);
		}

		// ;)
		strcpy(&newFile[1], filename);
		strcat(newFile, getenv("HOME"));
	
		return newFile;
		
	}

	// filename i a pointer to a command line argument... better  to
	// copy this to its own chunk of memory
	newFile = (char*)malloc(sizeof(char) * len + 1);
	strcpy(newFile, filename);

	// otherwise we should be in the clear to just return the filename
	return newFile;
}

int
validateInputs(char *inputFile, char *outputFile, Config *config)
{
	struct stat statbuf;

	// we will not even consider reading from the output file
	int cmp = strcmp(outputFile, inputFile);
	if (cmp == 0) {
		// mildly scold user
		printf("Refusing to read from output file. Try again\n");
		return 0;
	}

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

	// regular files and directories are ok. anything else is not
	if ( S_ISREG(statbuf.st_mode) )
		config->flag_fileonly = 1;
	else if ( S_ISDIR(statbuf.st_mode) )
		config->flag_fileonly = 0;
	else {
		printf("That's an interesting file you've got there. Aborting.\n");
		return 0;
	}

	// make sure we have write access to working directory
	if ( access(".", F_OK|X_OK|R_OK|W_OK) == -1 ) {

		printf("Error: Need write access to working directory to proceed.\n");
		return 0;
	}
	// check to see if output file exists
	if ( access(outputFile, F_OK) != -1 ) {

		// if the output file exists, make sure user
		// acknowledges overwrite	
		char response = ' ';
		printf("It appears your output file already exists.\n"); 
		printf("Would you like to overwrite it?\n");
		while (1) {
			printf("Please enter y or n : ");
			response = getchar();
			while( fgetc(stdin) != '\n');
			printf("\n");	
			if (response == 'y' || response == 'Y') {

				break;
			}
			if (response == 'n' || response == 'N') {

				printf("Aborting.\n");
				return 0;
			}
			printf("That's not a valid option.\n");
		}

		// make sure we have write permission to the output file
		if ( access(outputFile, F_OK|W_OK|R_OK) == -1 ) {
			printf("Bad permissions on output file. Aborting.\n");
			return 0;
		}
	}
	return 1;
}
