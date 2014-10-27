#include "header/readimage.h"
#include "header/mainwindow.h"

#include <iup.h>

#include <im.h>
#include <im_image.h>
#include <im_process_loc.h>

#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

//Put these into the Options dialog, not hard-coded
#define RESIZE_ORDER 0

int open_image_file(Ihandle* ih)
{
	//Show the open file dialog
	Ihandle* dlg = IupFileDlg();
	IupSetAttributes(dlg,"EXTFILTER=\"Common Picture Files|*.bmp;*.gif;*.jpg;*.jpeg;*.png|All Files|*.*|\", MULTIPLEFILES=YES");
	IupPopup(dlg,IUP_CURRENT,IUP_CURRENT);

	//If the user didn't cancel out of the dialog, continue
	if(IupGetInt(dlg,"STATUS") != 0)
		return IUP_DEFAULT;

	//If we have at least one file, continue
	char* fnames = IupGetAttribute(dlg,"VALUE");
	if(fnames == NULL)
		return IUP_DEFAULT;

	//Get the folder and all the files selected in that folder (if more than one file is selected)
	//WARNING! Heavy use of pointer arithmetic. Future me might get confused by this
	char temp[1024] = "", *fnamesFill, *tempFill;
	for(fnamesFill = fnames, tempFill = temp; *fnamesFill; fnamesFill++)
	{
		if(*fnamesFill == '|')
		{
			//Replace the | with a null character for easier string copying
			*fnamesFill = '\0';
			strcpy(tempFill,fnames);

			//Special case if this is the first string - that's our folder
			if(temp == tempFill)
			{
				tempFill += fnamesFill - fnames + 1;
				#ifdef WIN32
					*(tempFill - 1) = '\\';
				#else
					*(tempFill - 1) = '/';
				#endif
			} else {
				//Parse the filename
				parse_image_file(ih, temp);
			}

			fnames = fnamesFill + 1;
		}
	}

	//If there's just one file, parse it
	if(temp[0] == NULL)
		parse_image_file(ih,fnames);

	IupDestroy(dlg);
	return IUP_DEFAULT;
}

void parse_image_file(Ihandle* ih, const char* name)
{
	char msg[1024];
	int err;
	sprintf(msg,"Opening file %s... ", name);
	log_console(msg);
	imImage* img = imFileImageLoadBitmap(name, 0, &err);
	if(err != IM_ERR_NONE)
	{
		sprintf(msg, "FAIL: Error %d\n", err);
		log_console(msg);
		return;
	}
	log_console("\n");

	imImage *temp = imImageClone(img);
	imImageReshape(temp, 128, 128);
	imProcessResize(img, temp, RESIZE_ORDER);
	add_image(temp);
}
