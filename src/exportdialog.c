#include "header/exportdialog.h"

//static char directory[1024] = "", format[128] = "map_%d.dat";
static Ihandle *dir, *frmt, *result, *count, *export, *dlg;
static int nOfImages;

static char _is_directory(const char* path)
{
	static struct stat s;
	return (stat(path, &s) == 0 && (s.st_mode & S_IFDIR)) ? 1 : 0;
}

static int _set_status(Ihandle* ih)
{
	static char buf[2048];
	const char* path = IupGetAttribute(dir, "VALUE");
	if(!_is_directory(path))
		sprintf(buf, "\"%s\" is not a valid folder", path);
	else
	{
		char* format = IupGetAttribute(frmt, "VALUE");

		//Ensure that the format is in a proper format, i.e. has a %d
		char* pos = strstr(format, "%");
		if(!pos || *(pos+1) != 'd')
			sprintf(buf, "The format \"%s\" is invalid as it does not contain a %%d", format);
		else
		{
			char start_file[256], end_file[256];
			int from = IupGetInt(count, "VALUE");
			sprintf(start_file, format, from);
			sprintf(end_file, format, from+nOfImages);
			sprintf(buf, "Maps will be saved to %s"PATHSEP"%s through %s"PATHSEP"%s", path, start_file, path, end_file);
		}
	}

	IupSetAttribute(result, "TITLE", buf);

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
	_set_status(ih);
	return IUP_DEFAULT;
}

int export_dialog_folder(Ihandle* ih)
{
	//Make sure an image is selected
	LinkedList* ll = main_window_get_images_if_populated();
	if(!ll)
		return IUP_DEFAULT;

	//Directory textbox
	dir = IupText(NULL);
	IupSetAttribute(dir, "VALUE", "");//directory);
	IupSetAttributes(dir, "EXPAND=HORIZONTAL, NC=1024");
	IupSetCallback(dir, "VALUECHANGED_CB", (Icallback)_set_status);

	//Browse button
	Ihandle* button_browse = IupButton("...", NULL);
	IupSetCallback(button_browse, "ACTION", _browse_folder);

	//Format textbox
	frmt = IupText(NULL);
	IupSetAttribute(frmt, "VALUE", "map_%d.dat");//format);
	IupSetAttributes(frmt, "VISIBLECOLUMNS=10, NC=128");
	IupSetCallback(frmt, "VALUECHANGED_CB", (Icallback)_set_status);

	//Counter start textbox
	count = IupText(NULL);
	IupSetAttribute(count, "SPIN", "YES");
	IupSetInt(count, "SPINMAX", INT_MAX);
	IupSetCallback(count, "VALUECHANGED_CB", (Icallback)_set_status);

	//Result label
	result = IupLabel("");
	IupSetAttribute(result, "EXPAND", "HORIZONTAL");
	_set_status(NULL);

	//Cancel button
	Ihandle* cancel = IupButton("Cancel", NULL);
	IupSetCallback(cancel, "ACTION", (Icallback)IupExitLoop);

	//Export button
	export = IupButton("Export", NULL);
	//ACTION->

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
					cancel,
					NULL
			)
	), "TITLE=\"Export as Matrix\", DIALOGFRAME=YES, HIDETASKBAR=YES, RASTERSIZE=640x");
	IupSetAttributeHandle(dlg, "DEFAULTESC", cancel);

	IupPopup(dlg, IUP_CURRENT, IUP_CURRENT);
	IupDestroy(dlg);
	return IUP_DEFAULT;
}

