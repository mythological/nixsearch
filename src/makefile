CC             = gcc
CFLAGS         =-Wall -g
CFU_OFILES     =  cfu.o cfuhash.o cfulist.o cfustring.o
AUX_OFILES     = fileNode.o occurrences.o command_t.o sorted-list.o
SEARCH_OFILES  = search_functions.o inv_index_t.o
INDEXER_OFILES = run_indexer.o indexer.o PathStack.o St4x.o
OFILES         = main.o $(CFU_OFILES) $(AUX_OFILES) $(SEARCH_OFILES) $(INDEXER_OFILES)

.PHONY: all clean

all: post-build

pre-build:
	@echo
	@echo
	@echo Settings things up . . .
	@echo
	@echo

post-build: main-build
	@echo
	@echo
	@echo Build Successful!
	@echo
	@echo

main-build: pre-build
	@$(MAKE) --no-print-directory target

target: $(OFILES)
	$(CC) $(CFLAGS) $(OFILES) -o ../search

main: main.c
	$(CC) $(CFLAGS) -c main.c 

cfu: cfu.c cfuhash.c cfulist.c cfustring.c
	$(CC) $(CFLAGS) -c cfu.c -c cfuhash.c -c cfulist.c cfustring.c 

search_functions: search_functions.c
	$(CC) $(CFLAGS) -c search_functions 

inv_index_t: inv_index_t.c
	$(CC) $(CFLAGS) -c inv_index_t.c 

sorted-list: sorted-list.c
	$(CC) $(CFLAGS) -c sorted-list.c 

command_t: command_t.c
	$(CC) $(CFLAGS) -c sorted-list.c 

fileNode: fileNode.c
	$(CC) $(CFLAGS) -c fileNode.c 

occurrences: occurrences.c
	$(CC) $(CFLAGS) -c occurrences.c 

run_indexer: run_indexer.c
	$(CC) $(CFLAGS) -c run_indexer.c 

indexer: indexer.c
	$(CC) $(CFLAGS) -c indexer.c 

PathStack: PathStack.c
	$(CC) $(CFLAGS) -c PathStack.c 

St4x: St4x.c
	$(CC) $(CFLAGS) -c St4x.c 

clean:
	rm *.o
