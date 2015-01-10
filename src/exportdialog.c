#include "header/exportdialog.h"
#include "header/mainwindow.h"

#include <iup.h>

#include <stdio.h>
#include <sys/stat.h>

//static char directory[1024] = "", format[128] = "map_%d.dat";
static Ihandle *dir, *frmt, *result, *count;
static int nOfImages;

char _is_directory(const char* path)
{
	static struct stat s;
	return (stat(path, &s) == 0 && (s.st_mode & S_IFDIR)) ? 1 : 0;
}

int _set_status(Ihandle* ih)
{
	static char buf[2048];
	const char* path = IupGetAttribute(dir, "VALUE");
	if(!_is_directory(path))
		sprintf(buf, "\"%s\" is not a valid folder", path);
	else
	{
		char start_file[256], end_file[256];
		char* format = IupGetAttribute(frmt, "VALUE");
		int from = IupGetInt(count, "VALUE");
		int start = sprintf(start_file, format, from);
		int end = sprintf(end_file, format, from+nOfImages);
		sprintf(buf, "Maps will be saved to files %s"PATHSEP"%s to %s"PATHSEP"%s", path, (start<0)?start_file:format, path, (end<0)?end_file:format);
	}

	IupSetAttribute(result, "TITLE", buf);

	return IUP_DEFAULT;
}

int export_dialog_folder(Ihandle* ih)
{
	//Make sure an image is selected

	//Directory textbox
	dir = IupText(NULL);
	//IupSetAttribute(dir, "VALUE", directory);
	IupSetAttributes(dir, "VISIBLECOLUMNS=30, NC=1024");
	IupSetCallback(dir, "VALUECHANGED_CB", (Icallback)_set_status);

	//Browse button
	Ihandle* button_browse = IupButton("...", NULL);
	//ACTION_CB -> _browse_folder()

	//Format textbox
	frmt = IupText(NULL);
	IupSetAttribute(frmt, "VALUE", "map_%d.dat");//format);
	IupSetAttributes(frmt, "VISIBLECOLUMNS=10, NC=128");
	IupSetCallback(frmt, "VALUECHANGED_CB", (Icallback)_set_status);

	//Counter start textbox
	count = IupText(NULL);
	IupSetAttribute(count, "SPIN", "YES");
	//Set SPINMAX to 65535-image count
	IupSetCallback(count, "VALUECHANGED_CB", (Icallback)_set_status);

	//Result label
	result = IupLabel("");
	IupSetAttribute(result, "EXPAND", "HORIZONTAL");
	_set_status(NULL);

	Ihandle* dlg = IupSetAttributes(IupDialog(
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
					NULL
			)
	), "TITLE=\"Export as Matrix\", DIALOGFRAME=YES, HIDETASKBAR=YES");
	IupPopup(dlg, IUP_CURRENT, IUP_CURRENT);

	IupDestroy(dlg);
	return IUP_DEFAULT;
}

