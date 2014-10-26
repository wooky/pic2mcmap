#include "header/readimage.h"

#include <iup.h>
#include <iupcontrols.h>
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
	int i, memsize_f = 0, nOfElements = 0, section = 0;
	char temp[1024];
	for(i = 0; fnames[i]; i++, section += sizeof(char))
	{
		if(fnames[i] == '|')
		{
			//Special case if this is the first string - that's our folder
			if(temp[0] == NULL)
			{
				memsize_f = i+1;
				memcpy(temp,fnames,i * sizeof(char));
				#ifdef WIN32
					temp[i] = '\\';
				#else
					temp[i] = '/';
				#endif
			} else {
				//Put the full path into the string and that into the array
				memcpy(temp + memsize_f, fnames + i - section + 1, (section - 1) * sizeof(char));
				temp[i + section] = '\0';
				parse_image_file(ih, temp);

				nOfElements++;
			}

			section = 0;
		}
	}

	//If there's just one file, make it so
	if(nOfElements == 0)
		parse_image_file(ih,fnames);

	IupDestroy(dlg);
	return IUP_DEFAULT;
}

void parse_image_file(Ihandle* ih, char* name)
{
	printf("%s\n",name);
}
