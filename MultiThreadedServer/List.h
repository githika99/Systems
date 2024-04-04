/*
Githika Annapureddy
CruzId: gannapur
Assignment: pa4
*/
//-----------------------------------------------------------------------------
// LINKED LIST List.h
// Header file for List ADT
//-----------------------------------------------------------------------------
#ifndef LIST_H_INCLUDE_
#define LIST_H_INCLUDE_
#include <stdbool.h>

// Exported types -------------------------------------------------------------
typedef void *ListElement;
typedef struct ListObj *List;

// Constructors-Destructors ---------------------------------------------------

// newList()
// Returns reference to new empty List object.
List newList();

// freeList()
// Frees all heap memory associated with List *pL, and sets *pL to NULL.
void freeList(List *pL);

// Access functions -----------------------------------------------------------

// Returns the number of elements in L.
int length(List L);

// Returns index of cursor element if defined, -1 otherwise.
int cursor(List L);

// Returns front element of L. Pre: length()>0
void *front(List L);

// Returns back element of L. Pre: length()>0
void *back(List L);

// Returns cursor element data1 of L. Pre: length()>0, index()>=0
void *get1(List L);

// Returns cursor element data2 of L. Pre: length()>0, index()>=0
void *get2(List L);

// isEmpty is a helper function that I am not including in the .h file
// but I am including it in List.c

// Manipulation procedures ----------------------------------------------------

// Resets L to its original empty state.
void clear(List L);

// Overwrites the cursor element’s data with x.
// Pre: length()>0, index()>=0
void set(List L, void *data1, void *data2);

// If L is non-empty, sets cursor under the front element,
// otherwise does nothing.
void moveFront(List L);

// If L is non-empty, sets cursor under the back element,
// otherwise does nothing.
void moveBack(List L);

// If cursor is defined and not at front, move cursor one
// step toward the front of L; if cursor is defined and at
// front, cursor becomes undefined; if cursor is undefined
// do nothing
void movePrev(List L);

// If cursor is defined and not at back, move cursor one
// step toward the back of L; if cursor is defined and at
// back, cursor becomes undefined; if cursor is undefined
// do nothing
void moveNext(List L);

// Insert new element into L. If L is non-empty,
// insertion takes place before front element.
void prepend(List L, void *data1, void *data2);

// Insert new element into L. If L is non-empty,
// insertion takes place after back element.
void append(List L, void *data1, void *data2);

// Insert new element before cursor.
// Pre: length()>0, index()>=0
void insertBefore(List L, void *data1, void *data2);

// Insert new element after cursor.
// Pre: length()>0, index()>=0
void insertAfter(List L, void *data1, void *data2);

// Delete the front element. Pre: length()>0
void deleteFront(List L);

// Delete the back element. Pre: length()>0
void deleteBack(List L);

// Delete cursor element, making cursor undefined.
// Pre: length()>0, index()>=0
void delete (List L);

#endif
