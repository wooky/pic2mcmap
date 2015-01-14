#ifndef SRC_HEADER_LINKEDLIST_H_
#define SRC_HEADER_LINKEDLIST_H_

#include "imageutil.h"

#include <iupim.h>

#include <stdlib.h>
#include <stdio.h>

#define LL_APPEND INT_MAX

typedef struct LinkedList {
	unsigned char rows, cols;

	imImage* contents;
	imImage* thumbnail;
	imImage** grid;

	Ihandle* iContents;
	Ihandle* iThumbnail;
	Ihandle** iGrid;

	struct LinkedList *next;
} LinkedList;

LinkedList* LL_get(LinkedList*, int);
LinkedList* LL_insert(LinkedList**, imImage*, int);
void LL_remove(LinkedList**, int);
void LL_purge(LinkedList**);

#endif /* SRC_HEADER_LINKEDLIST_H_ */
