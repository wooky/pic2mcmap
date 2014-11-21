#include "header/linkedlist.h"

#include <iup.h>
#include <stdlib.h>

//Inserts an imImage before the given index
//Given a very big number, it'll just append to the list
//REQUIRES:	loc		A LinkedList pointer; if index is positive, give the head; otherwise, give the tail
//			image	The image to be inserted
//			index	Where to insert the image
void LL_insert(Ihandle* parent, LinkedList* loc, imImage* image, int index)
{
	int i;
	LinkedList *old_loc, *new_loc;

	LinkedList *img = malloc(sizeof(LinkedList));
	img->contents = image;

	//Seek out the address after which to insert the image
	for(i = 0, old_loc = NULL, new_loc = loc; i < index; i++, old_loc = new_loc, new_loc = new_loc->next)
	{
		//If the current location is NULL, we went too far, so we'll assume we're going to append to it
		if(new_loc == NULL)
			break;
	}

	img->next = new_loc;

	//If the old location is null, then this becomes the head
	if(old_loc == NULL)
		loc = img;
	//Otherwise, link the old location to this one
	else
		old_loc->next = img;
}

//This will not delete the actual content
void LL_remove(Ihandle* parent, LinkedList* head, int index)
{

}

//Display an error if the accessed linked list is invalid
void LL_error(Ihandle* parent)
{
	Ihandle* dlg = IupMessageDlg();
	IupPopup(
		IupSetAttributes(dlg,
			"TITLE=\"List Access Violation\",VALUE=\"The program accessed an invalid address while using the linked list\","
			"DIALOGTYPE=ERROR"
		), 0, 0
	);
	IupDestroy(dlg);
}
