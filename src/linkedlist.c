#include "header/linkedlist.h"
#include "header/mainwindow.h"

#include <iup.h>
#include <stdlib.h>
#include <stdio.h>

//Throw an error
void throw_error(int i, int index)
{
	char buf[256];
	sprintf(buf, "[ERROR] Cannot delete element %d, only %d elements exist!\n", index, i);
}

//Get a linked list by index, or throw an error on NULL
LinkedList* LL_get(LinkedList* head, int index)
{
	int i;
	LinkedList *old_loc, *new_loc;
	for(i = 0, old_loc = NULL, new_loc = head; i < index; i++, old_loc = new_loc, new_loc = new_loc->next)
	{
		//If the current location is NULL, we have gone too far, so throw an error
		if(new_loc == NULL)
		{
			throw_error(i,index);
			return NULL;
		}
	}

	//If the current location is still NULL, throw an error
	if(new_loc == NULL)
	{
		throw_error(i,index);
		return NULL;
	}

	//Return the list
	return new_loc;
}

//Inserts an imImage before the given index
//Given a very big number, it'll just append to the list
//REQUIRES:	loc		A LinkedList pointer; if index is positive, give the head; otherwise, give the tail
//			image	The image to be inserted
//			index	Where to insert the image
void LL_insert(LinkedList** loc, imImage* image, int index)
{
	int i;
	LinkedList *old_loc, *new_loc;

	LinkedList *img = malloc(sizeof(LinkedList));
	img->contents = image;

	//Seek out the address after which to insert the image
	for(i = 0, old_loc = NULL, new_loc = *loc; i < index; i++, old_loc = new_loc, new_loc = new_loc->next)
	{
		//If the current location is NULL, we went too far, so we'll assume we're going to append to it
		if(new_loc == NULL)
			break;
	}

	img->next = new_loc;

	//If the old location is null, then this becomes the head
	if(old_loc == NULL)
		*loc = img;
	//Otherwise, link the old location to this one
	else
		old_loc->next = img;
}

//Deletes the linked list and its contents
void LL_remove(LinkedList* head, int index)
{
	int i;
	LinkedList *old_loc, *new_loc;
	for(i = 0, old_loc = NULL, new_loc = head; i < index; i++, old_loc = new_loc, new_loc = new_loc->next)
	{
		//If the current location is NULL, we have gone too far, so throw an error
		if(new_loc == NULL)
		{
			throw_error(i,index);
			return;
		}
	}

	//If the current location is still NULL, throw an error
	if(new_loc == NULL)
	{
		throw_error(i,index);
		return;
	}

	//Replace the previous entry to the one over
	if(old_loc != NULL)
		old_loc->next = new_loc->next;

	//Delete the current entry
	free(new_loc);
}

void LL_purge(LinkedList* head)
{
	LinkedList* loc = head;
	while(loc != NULL)
	{
		LinkedList* old = loc;
		loc = loc->next;
		imImageDestroy(old->contents);
		free(old);
	}
	head = NULL;
}
