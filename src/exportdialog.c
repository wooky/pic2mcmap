#include "header/exportdialog.h"

static Ihandle *dir, *frmt, *result, *count, *export, *dlg, *button_browse, *cancel;
static LinkedList* ll;
static int nOfImages;
static char buf[2048];
static unsigned short from, to;

static int _do_export(Ihandle* ih)
{
	buf_msg_init("Error Saving Map(s)", "The following error(s) occurred while saving to map files:");
	status_bar_init("Saving map cluster...");
	status_bar_count(nOfImages);

	char *format = IupGetAttribute(frmt, "VALUE");
	sprintf(buf, "%s"PATHSEP, IupGetAttribute(dir, "VALUE"));
	int len = strlen(buf);
	short i;
	for(i = 0; i < nOfImages; i++)
	{
		sprintf(buf+len, format, i + from);
		int err = imFileImageSave(buf, "DAT", ll->grid[i]);
		if(err != IM_ERR_NONE)
		{
			int l = strlen(buf);
			sprintf(buf+l, "The following error occurred while saving to %s: %s (%d)\n", buf, imIupErrorMessage(err), err);
			buf_msg_put(buf+l);
		}
		status_bar_inc();
	}

	status_bar_done();
	buf_msg_show();
	return IUP_CLOSE;
}

static int _do_export_to_world(Ihandle* ih)
{
	//Define the NBT to be written to idcounts.dat
#define UNKNOWN 0
#define NBT_OUT_SIZE 12
	static unsigned char nbt_output[NBT_OUT_SIZE] = {
			TAG_COMPOUND, 0, 0,									//root tag, unnamed
				TAG_SHORT, 0, 3, 'm', 'a', 'p', UNKNOWN, UNKNOWN,	//current map count - unknown at this time
			TAG_INVALID											//END root tag
	};

	//Set the last map number to be written
	nbt_output[9] = (to>>8) & 0xff;
	nbt_output[10] = to & 0xff;

	//Write the NBT to idcounts.dat
	sprintf(buf, "%s"PATHSEP"data"PATHSEP"idcounts.dat", IupGetAttribute(dir, "VALUE"));
	FILE *idcounts = fopen(buf, "wb");
	if(idcounts == NULL)
	{
		show_warning("File Access Failure", "Could not write to the idcounts.dat file. Make sure you have writing permission and that it's not used by another program.");
		return IUP_DEFAULT;
	}
	fwrite(nbt_output, sizeof(unsigned char), NBT_OUT_SIZE, idcounts);
	fclose(idcounts);

	//Now dump the images into the data directory
	sprintf(buf, "%s"PATHSEP"data", IupGetAttribute(dir, "VALUE"));
	IupSetAttribute(dir, "VALUE", buf);
	return _do_export(ih);
}

static char _is_directory(const char* path)
{
	static struct stat s;
	return (stat(path, &s) == 0 && (s.st_mode & S_IFDIR)) ? 1 : 0;
}

static int _set_status_for_world(Ihandle* ih)
{
	const char* path = IupGetAttribute(dir, "VALUE");
	if(!_is_directory(path))
	{
		sprintf(buf, "\"%s\" is not a valid folder", path);
		IupSetAttribute(export, "ACTIVE", "NO");
	}
	else
	{
		char data[2048];
		sprintf(data, "%s"PATHSEP"data", path);
		if(!_is_directory(data))
		{
			sprintf(buf, "\"%s\" does not appear to be a valid Minecraft save directory", path);
			IupSetAttribute(export, "ACTIVE", "NO");
		}
		else
		{
			//Open idcounts.dat, if it exists, and determine the ID from which to start counting
			sprintf(buf, "%s"PATHSEP"idcounts.dat", data);
			from = 0;
			FILE *idcounts = fopen(buf, "rb");
			if(idcounts != NULL) {
				fseek(idcounts, 0, SEEK_END);
				size_t length = ftell(idcounts);
				fseek(idcounts, 0 , SEEK_SET);

				fread(buf, sizeof(char), length, idcounts);
				nbt_node *nbt = nbt_parse(buf, length);
				nbt_node *map = nbt_find_by_name(nbt, "map");
				if(map != NULL)
					from = ++map->payload.tag_short;

				nbt_free(nbt);
				fclose(idcounts);
			}

			IupSetInt(count, "SPINVALUE", (int)from);
			to = from + nOfImages - 1;
			sprintf(buf, "Maps will be saved to %s"PATHSEP"map_%hi.dat through %s"PATHSEP"map_%hi.dat", data, from, data, to);
			IupSetAttribute(export, "ACTIVE", "YES");
		}
	}

	IupSetAttribute(result, "VALUE", buf);
	return IUP_DEFAULT;
}

static int _set_status_for_dir(Ihandle* ih)
{
	const char* path = IupGetAttribute(dir, "VALUE");
	if(!_is_directory(path))
	{
		sprintf(buf, "\"%s\" is not a valid folder", path);
		IupSetAttribute(export, "ACTIVE", "NO");
	}
	else
	{
		char* format = IupGetAttribute(frmt, "VALUE");

		//Ensure that the format is in a proper format, i.e. has a %d
		char* pos = strstr(format, "%");
		if(!pos || *(pos+1) != 'd')
		{
			sprintf(buf, "The format \"%s\" is invalid as it does not contain a %%d", format);
			IupSetAttribute(export, "ACTIVE", "NO");
		}
		else
		{
			char start_file[256], end_file[256];
			from = (unsigned short)IupGetInt(count, "VALUE");
			sprintf(start_file, format, from);
			to = from + nOfImages - 1;
			sprintf(end_file, format, to);
			sprintf(buf, "Maps will be saved to %s"PATHSEP"%s through %s"PATHSEP"%s", path, start_file, path, end_file);
			IupSetAttribute(export, "ACTIVE", "YES");
		}
	}

	IupSetAttribute(result, "VALUE", buf);
	return IUP_DEFAULT;
}

static int _browse_folder(Ihandle* ih)
{
	Ihandle* derp = IupFileDlg();
	IupSetAttribute(derp, "DIALOGTYPE", "DIR");
	IupPopup(derp, IUP_CURRENT, IUP_CURRENT);

	if(IupGetInt(derp, "STATUS") != -1)
		IupSetAttribute(dir, "VALUE", IupGetAttribute(derp, "VALUE"));

	IupDestroy(derp);
	IupGetCallback(dir, "VALUECHANGED_CB")(ih);
	return IUP_DEFAULT;
}

char _setup_dialog(Icallback status_callback, Icallback export_callback)
{
	//Make sure an image is selected
	ll = main_window_get_images_if_populated();
	if(!ll)
		return 0;

	//Get the image count
	nOfImages = ll->rows * ll->cols;

	//Directory textbox
	dir = IupText(NULL);
	IupSetAttribute(dir, "VALUE", "");
	IupSetAttributes(dir, "EXPAND=HORIZONTAL, NC=1024");
	IupSetCallback(dir, "VALUECHANGED_CB", status_callback);

	//Browse button
	button_browse = IupButton("...", NULL);
	IupSetCallback(button_browse, "ACTION", _browse_folder);

	//Format textbox
	frmt = IupText(NULL);
	IupSetAttribute(frmt, "VALUE", "map_%d.dat");
	IupSetAttributes(frmt, "VISIBLECOLUMNS=10, NC=128");
	IupSetCallback(frmt, "VALUECHANGED_CB", status_callback);

	//Counter start textbox
	count = IupText(NULL);
	IupSetAttribute(count, "SPIN", "YES");
	IupSetInt(count, "SPINMAX", INT_MAX);
	IupSetCallback(count, "VALUECHANGED_CB", status_callback);

	//Result textbox
	result = IupText(NULL);
	IupSetAttributes(result, "EXPAND=HORIZONTAL, MULTILINE=YES, VISIBLELINES=2, ACTIVE=NO, SCROLLBAR=NO, WORDWRAP=YES");

	//Export button
	export = IupButton("Export", NULL);
	IupSetCallback(export, "ACTION", export_callback);

	//Cancel button
	cancel = IupButton("Cancel", NULL);
	IupSetCallback(cancel, "ACTION", (Icallback)IupExitLoop);

	return 1;
}

int export_dialog_world(Ihandle* ih)
{
	if(!_setup_dialog((Icallback)_set_status_for_world, (Icallback)_do_export_to_world))
		return IUP_DEFAULT;

	IupSetAttribute(frmt, "ACTIVE", "NO");
	IupSetAttribute(count, "ACTIVE", "NO");

	dlg = IupSetAttributes(IupDialog(
			IupVbox(
					IupSetAttributes(IupHbox(
							IupLabel("Select world save folder: "),
							dir,
							button_browse,
							NULL
					), "ALIGNMENT=ACENTER"),
					IupSetAttributes(IupHbox(
							IupLabel("Save in the format "),
							frmt,
							IupLabel(" where %d starts at "),
							count,
							NULL
					), "ALIGNMENT=ACENTER"),
					IupSetAttributes(IupFrame(
							result
					), "TITLE=Result"),
					IupHbox(
						export,
						IupFill(),
						cancel,
						NULL
					),
					NULL
			)
	), "TITLE=\"Export to World\", DIALOGFRAME=YES, HIDETASKBAR=YES");
	IupSetAttributeHandle(dlg, "DEFAULTESC", cancel);

	_set_status_for_world(NULL);
	IupPopup(dlg, IUP_CURRENT, IUP_CURRENT);

	IupDestroy(dlg);
	return IUP_DEFAULT;
}

int export_dialog_folder(Ihandle* ih)
{
	if(!_setup_dialog((Icallback)_set_status_for_dir, (Icallback)_do_export))
		return IUP_DEFAULT;

	dlg = IupSetAttributes(IupDialog(
			IupVbox(
					IupSetAttributes(IupHbox(
							IupLabel("Save to folder: "),
							dir,
							button_browse,
							NULL
					), "ALIGNMENT=ACENTER"),
					IupSetAttributes(IupHbox(
							IupLabel("Save in the format "),
							frmt,
							IupLabel(" where %d starts at "),
							count,
							NULL
					), "ALIGNMENT=ACENTER"),
					IupSetAttributes(IupFrame(
							result
					), "TITLE=Result"),
					IupHbox(
						export,
						IupFill(),
						cancel,
						NULL
					),
					NULL
			)
	), "TITLE=\"Export as Matrix\", DIALOGFRAME=YES, HIDETASKBAR=YES");
	IupSetAttributeHandle(dlg, "DEFAULTESC", cancel);

	_set_status_for_dir(NULL);
	IupPopup(dlg, IUP_CURRENT, IUP_CURRENT);

	IupDestroy(dlg);
	return IUP_DEFAULT;
}
