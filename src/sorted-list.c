
#include <stdlib.h>
#include <stdio.h>
#include "sorted-list.h"

/* Name: Node
 * Purpose: Represents a single node in the SortedList data type
 * Members: 
 *     void *data: Allows the node to contain data. The Nodes in
 *                 the sorted list are sorted using this field
 *     struct _Node next: A pointer to the next Node in the sorted
 *                        list
 *     int refCount: A running tally of the number of pointers
 *                   pointing to the Node
 */
typedef struct _Node {

	void *data;
	struct _Node  *next;
	int refCount;
} Node;

/* Name: SortedList
 * Purpose: The concrete data type for our Sorted List.
 * Members: 
 *     Node *front: A pointer to the first Node in our list
 *     CompareFucT comparator: A pointer to a generatic comparator
 *                             function. We use this to sort the list.
 *     DestructFuncT destroyer: A pointer to a Destructor fuction
 *                              specific to the data type we are
 *                              storing in our list
 */
struct SortedList {

	Node *front;
	CompareFuncT comparator;
	DestructFuncT destroyer;
};

/* Name: SortedListIterator
 * Purpose: Allows the caller to access the members of the SortedList struct
 * Members:
 *     Node *current: A pointer to the Node currently referenced by the iterator
 *     DestructFuctT destroyer: A pointer to a Destructor function specific to
 *                              the data type we are storing in our list
 */
struct SortedListIterator {

	Node *current;
	DestructFuncT destroyer;
};

// static prototypes
static Node *nodeFactory(void *newObj);

/* Name: SLCreate
 * Purpose: Given references to both a comparator function and a destructor,
 *          creates a new SortedList
 * Parameters:
 *          CompareFuncT cf: A pointer to a comparator function specific to
 *                           the data type that we plan on storing in the list
 *          DestructFuncT df: A pointer to a comparator function specific to
 *                           the data type that we plan on storing in the list
 * Returns:
 *          On success, returns a SortedListPtr .
 *          On failure, returns NULL.
 * BigO:
 *     SLCreate executes a finite number of instructions no matter
 *     the input size. This gives us O(1).
 */
SortedListPtr SLCreate(CompareFuncT cf, DestructFuncT df) {

	// return null if caller passes a null pointer as parameter
	if (!(cf && df))
		return NULL;

	// allocate memory for the struct SortedList
	SortedListPtr sl = (SortedListPtr)malloc(sizeof(struct SortedList));
	if (!sl) {
		fprintf(stderr,"Malloc failed in %s on line %d\n",__FILE__,__LINE__-2);
		return NULL;
	}

	// initialize member variables
	sl->comparator = cf;
	sl->destroyer = df;
	sl->front = NULL;

	// return a pointer to the newly created SortedList
	return sl;
}

/* Name: SLDestroy
 * Purpose: Given a pointer to a struct SortedList, frees all memory allocated
 *          to the list along with that of its nodes and members
 * Parameters:
 *     SortedListPtr list: A pointer to the sorted list that we are about to
 *                         destroy
 * Returns:
 *     void
 *Big O:
 *  Given a list of n nodes, SLDestroy must call free n times. This gives us O(n)
 */
void SLDestroy(SortedListPtr list) {
	// return if passed a null pointer
	if (!list)
		return;

	Node* curr = list->front;	
	Node* prev = NULL;

	// if list is empty, simply free the list and return
	if (!curr) {
		free(list);
		return;
	}

	// delete every item in the list
	while(curr) {
		
		prev = curr;
		curr = curr->next;
		if (prev->data)
			list->destroyer(prev->data);
		if (prev)
			free(prev);
	}

	free(list);
}

/* Name: SLInsert
 * Purpose: Given a pointer to struct SortedList list, and a pointer to
 *          an object newObj, inserts newObj into the list
 * Parameters:
 *     SortedListPtr list: A pointer to the list that we are inserting into
 *     void *newObj: The object that we are inserting into list
 * Returns:
 * 	   Type: int
 *     SUCCESS: 1
 *     FAILURE: 0
 * Big O:
 *	Best Case: O(1) newest object is the largst object, and becomes the new head
 *	worst Case: O(n) Smallest Object- needs to check every node and finally adds it to the end(The rear).
 *	The Big O would be O(n), Big O uses the worst case scenario and in the worst case the new object needs 
 *	to be placed at the end.
 */
int SLInsert(SortedListPtr list, void *newObj) {
	
	// return failture if passed a null pointer
	if (!list) {

		return 0;
	}
	if (!newObj) {

		return 0;
	}
	
	// create a new node
	Node *newNode = nodeFactory(newObj);
	if ( !newNode ) {
		perror("Unabel to create node...\n");
		return 0;
	}

	// if the list is empty, then set front to new node
	if (!list->front) {
		list->front = newNode;
		list->front->next = NULL;
		return 1;
	}



	// check for duplicate entries
	int result = list->comparator(list->front->data, newObj);

	/*
	if (!result) {
		free(newNode);
		perror("Cowardly refusing to insert a duplicate entry\n");
		return 0;
	}
	*/
	// if newObj greater or equal to than value of front, insert in front of list
	//else if (result <= 0) {
	if (result <= 0) {
		newNode->next = list->front;
		list->front = newNode;
		return 1;
	}

	// search for the current position in which to insert newObj
	Node *current = list->front->next;
	Node *prev = list->front;
	while(current) {

		// compare the value of current with the value of newObj
		result = list->comparator(current->data, newObj);

		/*
		// return failure if duplicate found
		if (!result) {
			free(newNode);
			perror("Cowardly refusing to insert a duplicate entry\n");
			return 0;
		}
		*/
		
		// if corrent position found, insert newObj
		if (result <= 0) {
			newNode->next = current;
			prev->next = newNode;
			return 1;
		}

		// traverse to the next Node in the list
		prev = current;
		current = current->next;
	}
	
	// if we've reached the end of the list, then append newObj
	// to the end of the list
	prev->next = newNode;
	return 1;
}

/* Name: nodeFactory
 * Purpose: Given an object newObj, allocates
 *          memory for a Node with value set to newObj
 * Parameters:
 *     void *newObj: A pointer to an object whose value will be
 *                   stored in the new Node
 * Returns:
 *     Type: Node*
 *     SUCCESS:
 *         A pointer to a new Node
 *     FAILURE:
 *         NULL
 */
static Node *nodeFactory(void *newObj) {
	// return NULL if passed a null pointer
	if (!newObj)
		return NULL;
	
	// allocate space for a new Node
	Node *tmp = (Node*)malloc(sizeof(Node));
	if (!tmp)
		return NULL;

	// initialize the member of the new Node
	tmp->data = newObj;
	tmp->next = NULL;
	tmp->refCount = 1;

	// return the Node
	return tmp;
}

/* Name: SLRemove
 * Purpose: Given a SortedListPtr list and void* delMe as
 *          parameters, remove the Node in list whose value
 *          is equal to that of *delMe
 * Parameters:
 *     SortedListPtr list: A pointer to the SortedList we are
 *                         removing from
 *     void *delMe: The value of the Node that we wish to delete
 * Returns:
 *     Type: int
 *     SUCCESS: 1
 *     FAILURE: 0
 * Big O
 *	Best Case: O(1) the head is the object which should be deleated
 *	Worst Case: O(n) the whole list needs to be transvesed through, 
 *	and the object to be deleted is either at the end or does not exist
 *	As such the the big O is O(n), there is no way to do worse than the whole list and there is no way to do better.
 */
int SLRemove(SortedListPtr list, void *delMe) {
	
	Node *temp, *current, *prev;

	// check for null pointers
	if (list == NULL)
		return 0;

	if (delMe == NULL)
		return 0;

	current = list->front;

	// no use trying to remove something from an empty list
	if (current == NULL)
		return 0;

	// there's one item left and it wasn't the target
	if (list->comparator(list->front->data, delMe) != 0 && list->front->next == NULL) {
		return 0;
	}

	
	// there's one item left and it's the target
	if (list->comparator(list->front->data, delMe) == 0 && list->front->next == NULL) {
		
		temp = list->front;
		list->front = NULL;
		temp->refCount--;
		if (temp->refCount == 0) {
			list->destroyer(temp->data);
			free(temp);
		}
		return 1;
	}

	// the target is at the front of the list
	if (list->comparator(list->front->data, delMe) == 0 && list->front->next != NULL) {
		temp = list->front;
		list->front = list->front->next;
		temp->refCount--;
		list->front->refCount++;
		if (temp->refCount == 0) {
			list->destroyer(temp->data);
			free(temp);
			list->front->refCount--;
		}
		return 1;
	}

	// at this point, we know the front is not the target and front->next is not null
	prev = list->front;
	current = list->front->next;
	while (current != NULL) {

		// check if current equal to target
		if (list->comparator(current->data, delMe) == 0) {

			if (current->next != NULL) {
			
				prev->next = current->next;
				current->refCount--;
				current->next->refCount++;
				if (current->refCount == 0) {
		
					list->destroyer(current->data);
					free(current);
					prev->next->refCount--;
				}
				return 1;
			}
			else {
	
				prev->next = NULL;
				current->refCount--;
				if (current->refCount == 0) {
					list->destroyer(current->data);
					free(current);
				}
				return 1;
			}
		}
		prev = current;
		current = current->next;
	}
	return 0;
}

/* Name: SLCreateIterator
 * Purpose: Given a sorted list, allocates memory for an
 *          iterator to that list and initializes member variables
 * Parameters: SortedListPtr list
 * Return:
 *     Type: SortedListIteratorPtr
 *     SUCCESS: Pointer to SortedListIterator
 *     FAILURE: NULL
 * BigO:
 *     SLNCreateIterator executes a finite number of instructions no matter
 *     the input size. This gives us O(1).
 */
SortedListIteratorPtr SLCreateIterator(SortedListPtr list) {

	// return NULL if passed a null terminator
	if (!list) {

		return NULL;
	}

	// allocate space for a SortedListIteratorPtr
	SortedListIteratorPtr iter;
	iter = (SortedListIteratorPtr)malloc(sizeof(struct SortedListIterator));
	if (!iter) {

		fprintf(stderr,"Malloc failed in %s on line %d\n",__FILE__,__LINE__-2);
		return NULL;
	}

	// check to see if the list is empty
	if (!list->front) {
		
		iter->current = NULL;
		return iter;
	}

	// point the iterator to the front of the list, and inrement the reference
	// count of the list->front
	iter->current = list->front;
	iter->current->refCount++;
	iter->destroyer = list->destroyer;
	return iter;
}

/* Name: SLDestroyIterator
 * Purpose: Given a SortedListIteratorPtr iter, delete the
 *          SortedListIterator pointed to by iter
 * Parameters:
 *     SortedListIteratorPtr iter: A pointer the iterator to be deleted
 * Returns:
 *     Void
 * Big Oh:
 *
 * When we call SLDestroyIterator() on iterator i, we must delete each node that is connected to
 * i and not connected to list sl or another iterator.  Assume there exists no other iterator, and that SLRemove()
 * has been called on every node list sl sequentially, such that sl is now completely empty and i points to the
 * front of a list of n nodes. Then a call to SLDestroy would iterate through each of these nodes, deleting each
 * one, until we reach the end of the list. This gives us worst case runtime of O(n).
 */
void SLDestroyIterator(SortedListIteratorPtr iter) {

	Node *temp;

	if (iter == NULL) {

		return;
	}

	if (iter->current == NULL) {
		free(iter);
		return;
	}

	if (iter->current->next == NULL) {
			
		temp = iter->current;		
		iter->current = NULL;
		temp->refCount--;
		if (temp->refCount == 0) {

			iter->destroyer(temp->data);
			free(temp);
		}
		free(iter);
		return;
	}
	do {

		temp = iter->current;
		iter->current = iter->current->next;
		temp->refCount--;
		iter->current->refCount++;

		if (temp->refCount == 0) {

			iter->destroyer(temp->data);
			free(temp);
			if (iter->current != NULL) {

				iter->current->refCount--;
			}
		}
	} while(iter->current != NULL && iter->current->refCount < 2);

	// at this point iter->current is either NULL or iter->current->refCount >= 2
	if (iter->current == NULL) {

		free(iter);
		return;
	}
	else {

		temp = iter->current;
		iter->current = NULL;
		temp->refCount--;
		free(iter);
		return;
	}
}

/* Name: SLGetItem
 * Purpose: Returns the value of the item currently being accessed by
 *          iterator
 * Parameters: 
 *          SortedListIteratorPtr iter: A pointer to an iterator to a
 *          sorted list
 * Returns: 
 *     Type: void*
 *     Success: The value of the Node acccessed by the iterator
 *     Failure: NULL
 * BigO:
 *     SLGetItem executes a finite number of instructions no matter
 *     the input size. This gives us O(1).
 */
void *SLGetItem( SortedListIteratorPtr iter ) {

	// ;)
	return !iter || !iter->current ? NULL : iter->current->data;
}

/* Name: SLNextItem
 * Purpose: Given a SortedListIteratorPtr iter, returns the value
 *          of next item in the list
 * Parameters: SortedListIteratorPtr iter
 * Returns:
 *     Type: void*
 *     Success: A pointer to the value of the next item in the list
 *     Failure: NULL
 * BigO:
 *     SLNextItem executes a finite number of instructions no matter
 *     the input size. This gives us O(1).
 */
void *SLNextItem(SortedListIteratorPtr iter) {

	Node *temp;

	// refuse to accept NULL pointer trollery
	if (iter == NULL) {

		return NULL;
	}

	if (iter->current == NULL) {

		return NULL;
	}

	// case: we have reached the end of the list
	if (iter->current->next == NULL) {
		
		temp = iter->current;
		iter->current = NULL;
		temp->refCount--;
		if (temp->refCount == 0) {
			
			iter->destroyer(temp->data);
			free(temp);
		}
		return NULL;
	}

	// case: we have not reached the end of the list
	temp = iter->current;
	iter->current = iter->current->next;
	temp->refCount--;
	iter->current->refCount++;
	if (temp->refCount != 0) {

		return iter->current->data;
	}
	else {
		iter->destroyer(temp->data);
		free(temp);
		iter->current->refCount--;
		return iter->current->data;
	}
}

void SLDebugPrint(SortedListPtr sl) {

	if (sl == NULL)
		return;
	if (sl->front == NULL)
		return;

	Node* current = sl->front;
	while (current) {
		printf("(D: %d, R: %d)->", *(int*)current->data, current->refCount);
		current = current->next;
	}
	printf("NULL\n");
}
