/*
Githika Annapureddy
CruzId: gannapur
Assignment: pa4
*/
//-----------------------------------------------------------------------------
// LINKED LIST List.c
// Implementation file for List ADT
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "List.h"

// structs --------------------------------------------------------------------

// private Node type
typedef struct NodeObj *Node;

// private NodeObj type
typedef struct NodeObj {
    void *data1;
    void *data2;
    Node prev;
    Node next;
} NodeObj;

// private ListObj type
typedef struct ListObj {
    Node front;
    Node back;
    int length;
    int index; //index of cursor
    Node cursor;
} ListObj;

// Helper Function that is not included in List.h----------------------

// Returns true if Q is empty, otherwise returns false.
bool isEmpty(List L) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling isEmpty() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    return (L->length == 0);
}

// Constructors-Destructors ---------------------------------------------------

// newNode()
// Returns reference to new Node object. Initializes next and data fields.
Node newNode(void *data1, void *data2) {
    Node N = malloc(sizeof(NodeObj));
    assert(N != NULL);
    N->data1 = data1;
    N->data2 = data2;
    N->prev = NULL;
    N->next = NULL;
    return (N);
}

// freeNode()
// Frees heap memory pointed to by *pN, sets *pN to NULL.
// does not actually free data1 and data2 objects, that must be done before calling freeNode
void freeNode(Node *pN) {
    if (pN == NULL || *pN == NULL) {
        fprintf(stderr, "Calling freeNode with Null Node reference");
        exit(EXIT_FAILURE);
    }
    free(*pN);
    *pN = NULL;
}

// newList()
// Returns reference to new empty List object.
List newList() {
    List L;
    L = malloc(sizeof(ListObj));
    assert(L != NULL);
    L->front = L->back = NULL;
    L->length = 0;
    L->cursor = NULL;
    L->index = -1;
    return (L);
}

// freeList()
// Frees all heap memory associated with List *pL, and sets *pL to NULL.
void freeList(List *pL) {
    if (pL == NULL || *pL == NULL) {
        fprintf(stderr, "Calling freeList with Null List reference");
        exit(EXIT_FAILURE);
    }
    while (!isEmpty(*pL)) {
        deleteFront(*pL); //instead of Dequeue()
    }
    free(*pL);
    *pL = NULL;
}

// Access functions -----------------------------------------------------------

// Returns the number of elements in L.
int length(List L) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling length() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    return (L->length);
}

// Returns index of cursor element if defined, -1 otherwise.
int cursor(List L) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling index() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (L->index < 0 || L->index >= L->length) {
        return (-1);
    }

    return (L->index);
}

// Returns front element of L
// Pre: !isEmpty(Q)
//returns only URI (data1) not rwlock (data2)
void *front(List L) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling front() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (isEmpty(L)) {
        fprintf(stderr, "List Error: calling front() on an empty List\n");
        exit(EXIT_FAILURE);
    }
    return (L->front->data1);
}

// Returns back element of L. Pre: length()>0
//returns only URI (data1) not rwlock (data2)
void *back(List L) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling front() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (isEmpty(L)) {
        fprintf(stderr, "List Error: calling front() on an empty List\n");
        exit(EXIT_FAILURE);
    }
    return (L->back->data1);
}

// Returns cursor element of L. Pre: length()>0, index()>=0
//returns only URI (data1)
void *get1(List L) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling get() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (isEmpty(L)) {
        fprintf(stderr, "List Error: calling get() on an empty List\n");
        exit(EXIT_FAILURE);
    }
    if (L->index < 0) {
        fprintf(stderr, "List Error: calling get() on an undefined cursor\n");
        exit(EXIT_FAILURE);
    }
    return (L->cursor->data1);
}

// Returns cursor element of L. Pre: length()>0, index()>=0
//returns rwlock (data2)
void *get2(List L) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling get() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (isEmpty(L)) {
        fprintf(stderr, "List Error: calling get() on an empty List\n");
        exit(EXIT_FAILURE);
    }
    if (L->index < 0) {
        fprintf(stderr, "List Error: calling get() on an undefined cursor\n");
        exit(EXIT_FAILURE);
    }
    return (L->cursor->data2);
}

// Manipulation procedures ----------------------------------------------------

// Resets L to its original empty state.
void clear(List L) {
    //first clear all elements of L
    if (L != NULL) {
        while (!isEmpty(L)) {
            deleteFront(L); //instead of Dequeue()
        }
    }
    // should be true after the loop: L-> front = L-> back = NULL
    L->cursor = NULL;
    L->index = -1;
}

// Overwrites the cursor element’s data with x.
// Pre: length()>0, index()>=0
//used to manipulate list
void set(List L, void *data1, void *data2) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling set() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (isEmpty(L)) {
        fprintf(stderr, "List Error: calling set() on an empty List\n");
        exit(EXIT_FAILURE);
    }
    if (L->index < 0) {
        fprintf(stderr, "List Error: calling set() on an undefined cursor\n");
        exit(EXIT_FAILURE);
    }
    L->cursor->data1 = data1;
    L->cursor->data2 = data2;
}

// If L is non-empty, sets cursor under the front element,
// otherwise does nothing.
void moveFront(List L) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling moveFront() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (isEmpty(L)) {
        L->index = -1;
        return;
    }
    L->index = 0;
    L->cursor = L->front;
}

// If L is non-empty, sets cursor under the front element,
// otherwise does nothing.
void moveBack(List L) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling moveBack() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (isEmpty(L)) {
        return;
        //don't cause error if isEmpty, just don't do anything
        fprintf(stderr, "List Error: calling moveBack() on an empty List\n");
        exit(EXIT_FAILURE);
    }
    L->index = L->length - 1;
    L->cursor = L->back;
}

// If cursor is defined and not at front, move cursor one
// step toward the front of L; if cursor is defined and at
// front, cursor becomes undefined; if cursor is undefined
// do nothing
void movePrev(List L) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling movePrev() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (L->index < 0) {
        return;
    }
    if (isEmpty(L)) {
        fprintf(stderr, "List Error: calling movePrev() on an empty List\n");
        exit(EXIT_FAILURE);
    }
    if (L->index == 0) {
        L->index = -1;
        L->cursor = NULL;
    } else {
        L->index--;
        L->cursor = L->cursor->prev;
    }
}

// If cursor is defined and not at back, move cursor one
// step toward the back of L; if cursor is defined and at
// back, cursor becomes undefined; if cursor is undefined
// do nothing
void moveNext(List L) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling moveNext() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (L->index < 0) {
        return;
    }
    if (isEmpty(L)) {
        fprintf(stderr, "List Error: calling moveNext() on an empty List\n");
        exit(EXIT_FAILURE);
    }
    if (L->index >= L->length - 1) {
        L->index = -1;
        L->cursor = NULL;
    } else {
        L->index++;
        L->cursor = L->cursor->next;
    }
}

// Insert new element into L. If L is non-empty,
// insertion takes place before front element.
void prepend(List L, void *data1, void *data2) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling append() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }

    Node N = newNode(data1, data2);

    if (isEmpty(L)) {
        L->front = L->back = N;
    } else {
        N->next = L->front;
        L->front->prev = N;
        //makes the current back 's next = N
        L->front = N;
        //makes the new back = N
    }
    L->index++;
    L->length++;
}

// Insert new element into L. If L is non-empty,
// insertion takes place after back element.
void append(List L, void *data1, void *data2) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling append() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }

    Node N = newNode(data1, data2);
    if (isEmpty(L)) {
        L->front = L->back = N;
    } else {
        N->prev = L->back;
        L->back->next = N;
        //makes the current back 's next = N
        L->back = N;
        //makes the new back = N
    }
    L->length++;
}

// Insert new element before cursor.
// Pre: length()>0, index()>=0
void insertBefore(List L, void *data1, void *data2) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling insertBefore() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (isEmpty(L)) {
        fprintf(stderr, "List Error: calling insertBefore() on an empty List\n");
        exit(EXIT_FAILURE);
    }
    if (L->index < 0) {
        fprintf(stderr, "List Error: calling insertBefore() on an undefined cursor\n");
        exit(EXIT_FAILURE);
    }
    Node N = newNode(data1, data2);
    if (L->index == 0) {
        N->next = L->cursor;
        L->cursor->prev = N;
        L->front = N;
    } else {
        Node P = NULL;
        P = L->cursor->prev;
        P->next = N;
        N->prev = P;
        L->cursor->prev = N;
        N->next = L->cursor;
    }

    L->length++;
    L->index++;
}

// Insert new element before cursor.
// Pre: length()>0, index()>=0
void insertAfter(List L, void *data1, void *data2) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling insertAfter() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (isEmpty(L)) {
        fprintf(stderr, "List Error: calling insertAfter() on an empty List\n");
        exit(EXIT_FAILURE);
    }
    if (L->index < 0) {
        fprintf(stderr, "List Error: calling insertAfter() on an undefined cursor\n");
        exit(EXIT_FAILURE);
    }
    Node N = newNode(data1, data2);
    if (L->index == L->length - 1) {
        N->prev = L->cursor;
        L->cursor->next = N;
        L->back = N;
    } else {
        Node A = NULL;
        A = L->cursor->next;
        A->prev = N;
        N->next = A;
        L->cursor->next = N;
        N->prev = L->cursor;
    }

    L->length++;
}

// Deletes data at front of L.
// Pre: !isEmpty(L)
void deleteFront(List L) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling deleteFront(List L) on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (isEmpty(L)) {
        fprintf(stderr, "List Error: calling deleteFront(List L) on an empty List\n");
        exit(EXIT_FAILURE);
    }
    Node N = NULL;
    N = L->front;
    //check this before we modify L->front
    if (L->cursor == L->front) {
        L->cursor = NULL;
        L->index = -1;
    } else {
        L->index--;
    }

    if (L->length > 1) {
        L->front = L->front->next;
        //do we have to do this, or will it automatically be handled by freeing L->front
        L->front->prev = NULL;
    } else {
        L->front = L->back = NULL;
    }

    L->length--;

    freeNode(&N);
}

// Deletes data at back of L.
// Pre: !isEmpty(L)
void deleteBack(List L) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling deleteBack() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (isEmpty(L)) {
        fprintf(stderr, "List Error: calling deleteBack() on an empty List\n");
        exit(EXIT_FAILURE);
    }
    Node N = NULL;
    N = L->back;

    //check this before modifying L->back
    if (L->cursor == L->back) {
        L->cursor = NULL;
        L->index = -1;
    }

    if (L->length > 1) {
        L->back = L->back->prev;
        L->back->next = NULL;
    } else {
        L->front = L->back = NULL;
    }
    L->length--;

    freeNode(&N);
}

// Delete cursor element, making cursor undefined.
// Pre: length()>0, index()>=0
void delete (List L) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling delete() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (isEmpty(L)) {
        fprintf(stderr, "List Error: calling delete() on an empty List\n");
        exit(EXIT_FAILURE);
    }
    if (L->index < 0) {
        fprintf(stderr, "List Error: calling delete() on an undefined cursor\n");
        exit(EXIT_FAILURE);
    }

    //edge cases:
    if (L->length == 1) {
        //printf("first condition");
        L->front = L->back = NULL;
        L->index = -1;
        free((L->cursor)); //added this
        L->cursor = NULL;
        L->length--; //since deleteFront and deleteBack decrement length, we must decrement it seperately here
    }
    //cursor was front
    else if (L->cursor == L->front) {
        //cursor is freed, cursor = NULL, length--, index = -1 in deleteFront
        deleteFront(L);
    }
    //cursor was back
    else if (L->cursor == L->back) {
        //cursor is freed, cursor = NULL, length--, index = -1 in deleteBack
        deleteBack(L);
    } else {
        L->cursor->prev->next = L->cursor->next;
        L->cursor->next->prev = L->cursor->prev;
        free((L->cursor));
        L->length--;
        L->index = -1;
        L->cursor = NULL;
    }
    //should I do free(L->cursor) or just set it to null
}

// Other Functions ------------------------------------------------------------

// Prints to the file pointed to by out, a
// string representation of L consisting
// of a space separated sequence of integers,
// with front on left.
// No longer in .h file, used to test only
/*
void printList(FILE* out, List L){
   if( L==NULL ){
      printf("List Error: calling printList() on NULL List reference\n");
      exit(EXIT_FAILURE);
   }

   moveFront(L);
   while(index(L) != -1){
      fprintf(out, "(%d, %f) ", ((Entry)get(L))-> row, ((Entry)get(L))-> data);
      moveNext(L);
   }
   fprintf(out, "\n");
}
*/
