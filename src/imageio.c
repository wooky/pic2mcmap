#include "header/imageio.h"

//Put these into the Options dialog, not hard-coded
#define RESIZE_ORDER 0

static int _closest128(int x)
{
	if(x <= 192)
		return 128;
	else
	{
		int y = x/128;
		return y*128;
	}
}

int image_open_file(Ihandle* ih)
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

	//Get the length of fnames, for later usage
	int flength = strlen(fnames);

	//We will be splitting the path into tokens, which are separated by "|"
#define NORMAL_OP 1
#define FINAL_OP 0
	fnames = strtok(fnames, "|");	//fnames gets altered anyways by strtok(), so we may as well reuse it

	//If the lengths of the strings are exactly the same, this means that we only have one file. So process it immediately.
	if(strlen(fnames) == flength)
		image_parse_file(ih, fnames, FINAL_OP, 0, 0);

	//Otherwise, go through every filename, concatenate it to the path, and parse it
	else
	{
		char path[1024];

		//Copy the directory, noting the path separator
		sprintf(path, "%s"PATHSEP, fnames);
		int len = strlen(path);

		//We need a token right now so that we know the first filename
		fnames = strtok(NULL, "|");

		while(fnames)
		{
			strcpy(path + len, fnames);
			fnames = strtok(NULL, "|");
			image_parse_file(ih, path, fnames ? NORMAL_OP : FINAL_OP, 0, 0);
		}
	}

	IupDestroy(dlg);
	main_window_set_menu_state();
	return IUP_DEFAULT;
}

//Parse the given filename
//3 last extra arguments to satisfy DROPFILES_CB, because I'm too lazy to write yet another function to call this one
//Pass NORMAL_OP to num for normal operation, otherwise weird stuff might happen
void image_parse_file(Ihandle* ih, const char* name, int num, int x, int y)
{
	//Initialize the buffered message
	buf_msg_init("Error Opening File(s)", "The following error(s) occurred while trying to open files:");

	char msg[128];
	int err;
	sprintf(msg,"Opening file %s...", name);
	status_bar_init(msg);
	imImage* img = imFileImageLoadBitmap(name, 0, &err);
	if(err != IM_ERR_NONE)
	{
		sprintf(msg, "Cannot open %s: %s (%d)", name, imIupErrorMessage(err), err);
		buf_msg_put(msg);
	}
	else if(img->width % 128 == 0 && img->height % 128 == 0)
		main_window_add_image(img);
	else
	{
		imImage *temp = imImageClone(img);
		imImageReshape(temp, _closest128(img->width), _closest128(img->height));
		imProcessResize(img, temp, RESIZE_ORDER);
		imImageDestroy(img);
		main_window_add_image(temp);
	}

	status_bar_done();

	//Show errors, if any, but only if this is the last file being processed
	if(num == 0)
		buf_msg_show();
}

//Save a file to a given list of formats
int image_save_file(Ihandle* ih)
{
	//Making sure we actually have an image selected
	LinkedList* ll = main_window_get_images_if_populated();
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
