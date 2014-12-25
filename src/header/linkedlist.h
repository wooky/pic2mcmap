#ifndef SRC_HEADER_LINKEDLIST_H_
#define SRC_HEADER_LINKEDLIST_H_

#include <iup.h>
#include <im.h>
#include <im_image.h>

#include <limits.h>

#define LL_APPEND INT_MAX

typedef struct LinkedList {
	unsigned char rows, cols;
	unsigned char* palette_indexes;

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
