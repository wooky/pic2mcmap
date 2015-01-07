#include "header/imageio.h"
#include "header/mainwindow.h"
#include "header/datformat.hpp"
#include "header/statusbar.h"
#include "header/bufmsg.h"
#include "header/imageutil.h"

#include <iup.h>

#include <im.h>
#include <im_image.h>
#include <im_process_loc.h>
#include <iupim.h>
#include <im_convert.h>
#include <im_util.h>

#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

//Put these into the Options dialog, not hard-coded
#define RESIZE_ORDER 0
#define NORMAL_OP -1

int closest128(int x)
{
	if(x <= 192)
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
	IupSetAttributes(dlg,"EXTFILTER=\"All Supported Files (*.bmp;*.gif;*.jpg;*.jpeg;*.png;*.dat)|*.bmp;*.gif;*.jpg;*.jpeg;*.png;*.dat|Common Picture Files (*.bmp;*.gif;*.jpg;*.jpeg;*.png)|"
			"*.bmp;*.gif;*.jpg;*.jpeg;*.png|Minecraft NBT Map Format (*.dat)|*.dat|All Files (*.*)|*.*|\", MULTIPLEFILES=YES");
	IupPopup(dlg,IUP_CURRENT,IUP_CURRENT);

	//If the user didn't cancel out of the dialog, continue
	if(IupGetInt(dlg,"STATUS") != 0)
	{
		IupDestroy(dlg);
		return IUP_DEFAULT;
	}

	//If we have at least one file, continue
	char* fnames = IupGetAttribute(dlg,"VALUE");
	if(fnames == NULL)
	{
		IupDestroy(dlg);
		return IUP_DEFAULT;
	}

	//Init the error dialog
	buf_msg_init("Error Opening File(s)", "The following error(s) occurred while trying to open files:");

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
				parse_image_file(ih, temp, NORMAL_OP, 0, 0);
			}

			fnames = fnamesFill + 1;
		}
	}

	//If there's just one file, parse it
	if(!temp[0])
		parse_image_file(ih,fnames, NORMAL_OP, 0, 0);

	buf_msg_show();
	IupDestroy(dlg);
	set_menu_state();
	return IUP_DEFAULT;
}

//Parse the given filename
//3 last extra arguments to satisfy DROPFILES_CB, because I'm too lazy to write yet another function to call this one
//Pass NORMAL_OP to num for normal operation, otherwise weird stuff might happen
void parse_image_file(Ihandle* ih, const char* name, int num, int x, int y)
{
	//Initialize the buffered message if we're called "in an unusual way"
	if(num != NORMAL_OP)
		buf_msg_init("Error Opening File(s)", "The following error(s) occurred while trying to open files:");

	char msg[128];
	int err;
	sprintf(msg,"Opening file %s... ", name);
	status_bar_init(msg);
	imImage* img = imFileImageLoadBitmap(name, 0, &err);
	if(err != IM_ERR_NONE)
	{
		sprintf(msg, "Cannot open %s: %s (%d)", name, imIupErrorMessage(err), err);
		buf_msg_put(msg);
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
		add_image(temp);
	}

	//Show errors, again, if this is called unusually
	if(num == 0)
		buf_msg_show();
}

//Save a file to a given list of formats
int save_file(Ihandle* ih)
{
	//Making sure we actually have an image selected
	if(!IupGetInt(list, "COUNT"))
		return IUP_DEFAULT;

	int val = IupGetInt(list,"VALUE");
	if(val <= 0)
		return IUP_DEFAULT;

	LinkedList* ll = LL_get(images, val-1);
	if(!ll)
		return IUP_DEFAULT;

	//Show the save file dialog
	Ihandle* dlg = IupFileDlg();
	IupSetAttributes(dlg, "DIALOGTYPE=SAVE, EXTFILTER=\"GIF (*.gif)|*.gif|PNG (bugged, don't use) (*.png)|*.png|Bitmap Image (*.bmp)|*.bmp|\"");
	IupPopup(dlg,IUP_CURRENT,IUP_CURRENT);

	//If the user didn't cancel out of the dialog, continue
	if(IupGetInt(dlg,"STATUS") == -1)
	{
		IupDestroy(dlg);
		return IUP_DEFAULT;
	}

	//If we have at least one file, continue
	char* fname = IupGetAttribute(dlg,"VALUE");
	if(fname == NULL)
	{
		IupDestroy(dlg);
		return IUP_DEFAULT;
	}

	//Get the format in which the image will be saved
	char msg[1024];
#define TYPE_SIZE 4
	char types[][TYPE_SIZE] = {"GIF", "PNG", "BMP"};
	char* ext = types[IupGetInt(dlg, "FILTERUSED") - 1];
	sprintf(msg, "Saving to %s.%s... ", fname, ext);
	status_bar_init(msg);

	//Combine the small images into one big image
	int cols = ll->cols, rows = ll->rows;
	status_bar_count(cols*rows);
	imImage* big = imImageCreateBased(ll->grid[0], cols*128, rows*128, -1, -1);
	int i, j;
	for(i = 0; i < rows; i++)
		for(j = 0; j < cols; j++)
		{
			imProcessAddMargins(ll->grid[i*cols + j], big, 128*j, 128*(rows-i-1));
			status_bar_inc();
		}

	//Actually save the image
	sprintf(msg, "%s.%s", fname, ext);
	int err = imFileImageSave(msg, ext, big) != IM_ERR_NONE;
	if(err != IM_ERR_NONE)
	{
		sprintf(msg, "The following error occurred while saving to %s.%s: %s (%d)\n", fname, ext, imIupErrorMessage(err), err);
		show_warning("Error Saving File", msg);
	}

	status_bar_done();
	imImageDestroy(big);
	IupDestroy(dlg);
	return IUP_DEFAULT;
}
