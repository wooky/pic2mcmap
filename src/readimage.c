#include "header/readimage.h"

#include <iup.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h> //temp

int open_image_file(Ihandle* ih)
{
	//Show the open file dialog
	Ihandle* dlg = IupFileDlg();
	IupSetAttributes(dlg,"EXTFILTER=\"Common Picture Files|*.bmp;*.gif;*.jpg;*.jpeg;*.png|All Files|*.*|\", MULTIPLEFILES=YES");
	IupPopup(dlg,IUP_CURRENT,IUP_CURRENT);

	//If we have at least one image, continue
	if(IupGetInt(dlg,"STATUS") != 0)
		return IUP_DEFAULT;

	//Do we have one file, or more than that?
	char* fnames = IupGetAttribute(dlg,"VALUE");
	if(fnames == NULL)
		return IUP_DEFAULT;

	//Put all files into an array, should there be more than one file
	int i, nOfElements = 0, section, memsize_f = 0;
	char *folder = NULL, **paths = NULL;
	for(i = 0; fnames[i]; i++, section += sizeof(char))
	{
		if(fnames[i] == '|')
		{
			//Special case if this is the first string - that's our folder
			if(folder == NULL)
			{
				memsize_f = (i + 1) * sizeof(char);	//CAUTION! Not null-terminated because we don't need it to be so
				folder = malloc(memsize_f);
				if(folder == NULL)
					out_of_memory(ih);
				memcpy(folder,fnames,i * sizeof(char));
				folder[i] = '/';
			} else {
				//Allocate memory for the filename and array of filenames
				int s = memsize_f + section * sizeof(char);
				char *t = malloc(s);
				paths = realloc(paths, (paths ? sizeof(paths) : 0) + sizeof(char**));
				if(t == NULL || paths == NULL)
					out_of_memory(ih);

				//Put the full path into the string and that into the array
				memcpy(t, folder, memsize_f);
				memcpy(t + memsize_f, fnames + i - section + 1, (section - 1) * sizeof(char));
				t[s-1] = '\0';
				paths[nOfElements] = t;

				nOfElements++;
			}

			section = 0;
		}
	}

	//If there's just one file, make it so
	if(nOfElements == 0)
	{
		paths = malloc(sizeof(char*));
		paths[0] = fnames;
		nOfElements++;
	}

	//TEST! Print out all the files
	for(i = 0; i < nOfElements; i++)
		printf("%s\n",paths[i]);

	//Done - clean up
	cleanup();
	IupDestroy(dlg);
	return IUP_DEFAULT;
}

void out_of_memory(Ihandle* parent)
{
	Ihandle* dlg = IupMessageDlg();
	IupSetAttributes(dlg,"TITLE=\"Out of Memory\", VALUE=\"Fatal error while loading images: out of memory!\", DIALOGTYPE=ERROR");
	IupSetAttributeHandle(dlg,"PARENTDIALOG",parent);
	IupPopup(dlg,IUP_CURRENT,IUP_CURRENT);

	cleanup();
	IupDestroy(dlg);
	IupDestroy(parent);
	IupClose();
	exit(-1);
}

void cleanup()
{

}
