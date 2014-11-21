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

void LL_insert(Ihandle*, LinkedList*, imImage*, int);
void LL_remove(Ihandle*, LinkedList*, int);
void LL_error(Ihandle*);

#endif /* SRC_HEADER_LINKEDLIST_H_ */
