#include "header/readimage.h"
#include "header/mainwindow.h"
#include "header/maputils.h"

#include <iup.h>

#include <im.h>
#include <im_image.h>
#include <im_process_loc.h>
#include <iupim.h>

#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

//Put these into the Options dialog, not hard-coded
#define RESIZE_ORDER 0

int closest128(int x)
{
	if(x <= 192)	//128*1.5
		return 128;
	else
	{
		int y = x/128;
		return y*128;
	}
}

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
	char msg[128];
	int err;
	sprintf(msg,"Opening file %s... ", name);
	log_console(msg);
	imImage* img = imFileImageLoad(name, 0, &err);
	if(err != IM_ERR_NONE)
	{
		sprintf(msg, "FAIL: Error %d\n", err);
		log_console(msg);
		return;
	}

	if(img->width % 128 == 0 && img->height % 128 == 0)
		add_image(img);
	else
	{
		imImage *temp = imImageClone(img);
		imImageReshape(temp, closest128(img->width), closest128(img->height));
		imProcessResize(img, temp, RESIZE_ORDER);
		imImageDestroy(img);
		sprintf(msg, "Image resized to %dX%d... ", temp->width, temp->height);
		log_console(msg);
		add_image(temp);
	}

	log_console("OK!\n");
}

imImage* get_image_thumbnail(const imImage* orig)
{
	imImage *temp = imImageClone(orig);
	imImageReshape(temp, 128, 128);
	imProcessResize(orig, temp, RESIZE_ORDER);
	return temp;
}

imImage** split_to_grid(const imImage* orig, unsigned char* rows, unsigned char* cols, unsigned char** indexes)
{
	unsigned char nCols = orig->width/128, nRows = orig->height/128, i,j;

	*cols = nCols;
	*rows = nRows;
	imImage** matrix = malloc(nCols * nRows * sizeof(imImage*));
	*indexes = malloc(nCols * 128 * nRows * 128 * sizeof(unsigned char));

	for(i = 0; i < nRows; i++)
	{
		for(j = 0; j < nCols; j++)
		{
			imImage* temp = imImageCreate(128, 128, orig->color_space, orig->data_type);
			imProcessCrop(orig, temp, j*128, (nRows-i-1)*128);
			//matrix[i*nCols + j] = mapify(temp, indexes[(i*nCols + j)*128]);
			imImageDestroy(temp);
		}
	}

	return matrix;
}

Ihandle** grid_images(const imImage** matrix, int size)
{
	Ihandle** handle = malloc(size * sizeof(Ihandle*));
	int i;

	for(i = 0; i < size; i++)
		handle[i] = IupImageFromImImage(matrix[i]);

	return handle;
}
