#include "linkedlist.h"

//Throw an error
static void _throw_error(int i, int index)
{
	printf("[ERROR] Cannot access element %d, only %d elements exist!\n", index, i);
}

//Clean inside the linked list and free the memory
static void _clean_inside(LinkedList* ll)
{
	imImageDestroy(ll->contents);
	imImageDestroy(ll->thumbnail);

	IupDestroy(ll->iContents);
	IupDestroy(ll->iThumbnail);

	int i;
	for(i = 0; i < ll->cols * ll->rows; i++)
	{
		imImageDestroy(ll->grid[i]);
		IupDestroy(ll->iGrid[i]);
	}

	free(ll);
}

//Get a linked list by index, or throw an error on NULL
LinkedList* LL_get(LinkedList* head, int index)
{
	int i;
	LinkedList *new_loc;
	for(i = 0, new_loc = head; i < index; i++, new_loc = new_loc->next)
	{
		//If the current location is NULL, we have gone too far, so throw an error
		if(new_loc == NULL)
		{
			_throw_error(i,index);
			return NULL;
		}
	}

	//If the current location is still NULL, throw an error
	if(new_loc == NULL)
	{
		_throw_error(i,index);
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
//RETURNS:	The just made linked list
LinkedList* LL_insert(LinkedList** loc, imImage* image, int index)
{
	int i;
	LinkedList *old_loc, *new_loc;

	LinkedList *img = malloc(sizeof(LinkedList));
	img->contents = image;
	img->thumbnail = util_get_thumbnail(image);
	img->grid = util_split_to_grid(image, &img->rows, &img->cols);

	img->iContents = IupImageFromImImage(image);
	img->iThumbnail = IupImageFromImImage(img->thumbnail);
	img->iGrid = util_grid_images(img->grid, (img->rows * img->cols));

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

	return img;
}

//Deletes the linked list and its contents
void LL_remove(LinkedList** head, int index)
{
	int i;
	LinkedList *old_loc, *new_loc;
	for(i = 0, old_loc = NULL, new_loc = *head; i < index; i++, old_loc = new_loc, new_loc = new_loc->next)
	{
		//If the current location is NULL, we have gone too far, so throw an error
		if(new_loc == NULL)
		{
			_throw_error(i,index);
			return;
		}
	}

	//If the current location is still NULL, throw an error
	if(new_loc == NULL)
	{
		_throw_error(i,index);
		return;
	}

	//If we're at the head, try to set the next value as the head (or NULL)
	if(new_loc == *head)
		*head = new_loc->next;
	//Otherwise, replace the previous entry to the one over
	else if(old_loc != NULL)	//TODO: checking if old_loc isn't NULL is redundant because we already checked that it's not at the head
		old_loc->next = new_loc->next;

	//Delete the current entry
	_clean_inside(new_loc);
}

void LL_purge(LinkedList** head)
{
	LinkedList* loc = *head;
	while(loc != NULL)
	{
		LinkedList* old = loc;
		loc = loc->next;
		_clean_inside(old);
	}
	*head = NULL;
}
