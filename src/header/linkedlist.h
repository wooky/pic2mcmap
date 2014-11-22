#ifndef SRC_HEADER_LINKEDLIST_H_
#define SRC_HEADER_LINKEDLIST_H_

#include <iup.h>
#include <im.h>
#include <im_image.h>

#include <limits.h>

#define LL_APPEND INT_MAX

typedef struct LinkedList {
	imImage* contents;
	struct LinkedList *next;
} LinkedList;

LinkedList* LL_get(LinkedList*, int);
void LL_insert(LinkedList**, imImage*, int);
void LL_remove(LinkedList*, int);
void LL_purge(LinkedList*);

#endif /* SRC_HEADER_LINKEDLIST_H_ */
