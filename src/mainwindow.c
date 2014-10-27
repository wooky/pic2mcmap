#include "header/mainwindow.h"
#include "header/readimage.h"

#include <iup.h>
#include <im_image.h>
#include <iupim.h>

#include <stddef.h>
#include <stdio.h>

Ihandle* console;	//Logging console
Ihandle* list;		//List of images opened

//Keyboard shortcuts. Format: {shortcut, callback}
Keyboard keyboard[] = {
	{iup_XkeyCtrl(K_O),&open_image_file},
	{iup_XkeyCtrl(K_I),&test},
	{iup_XkeyCtrl(K_W),NULL},
	{iup_XkeyCtrl(iup_XkeyShift(K_W)),NULL},
	{iup_XkeyCtrl(K_S),NULL},
	{iup_XkeyCtrl(iup_XkeyShift(K_S)),NULL},
	{(int)NULL}
};

//temporary, pls remove
int test(Ihandle* self)
{
	Ihandle* dlg = IupMessageDlg();
	IupSetAttribute(dlg,"VALUE","ich bin ein test");
	IupPopup(dlg,IUP_CURRENT,IUP_CURRENT);
	return IUP_DEFAULT;
}

Icallback k_any(Ihandle* ih, int c)
{
	int i;
	for(i = 0; keyboard[i].key; i++)
	{
		if(c == keyboard[i].key)
		{
			if(keyboard[i].cb != NULL)	//TEMPORARY, all keyboard shortcuts will be mapped eventually
			{
				return (Icallback)(keyboard[i].cb)();
			}
		}
	}

	return (Icallback)IUP_DEFAULT;
}

void create_mainwindow()
{
	Ihandle *menu, *sizer, *vbox, *win;

	//Array of menus. Format: {name, callback}
	MenuItem fmenu[] = {
		{"&Open...\tCtrl+O",(Icallback)open_image_file},
		{"&Import Map...\tCtrl+I",(Icallback)test},
		{SEPARATOR},
		{"&Close\tCtrl+W",NULL},
		{"C&lose All\tCtrl+Shift+W",NULL},
		{SEPARATOR},
		{"&Export to Map...\tCtrl+S",NULL},
		{"Export to &World...\tCtrl+Shift+S",NULL},
		{SEPARATOR},
		{"E&xit", (Icallback)IupExitLoop},
		{NULL}
	};

	MenuItem ftools[] = {
			{"&Options"},
			{NULL}
	};

	//Create the menu
	menu = IupMenu(
		create_submenu("&File",fmenu),
		create_submenu("&Tools",ftools),
		NULL
	);

	//Create the container to put all the stuff into
	console = IupText(NULL);
	IupSetAttributes(console,"MULTILINE=YES, READONLY=YES, MINSIZE=x85, EXPAND=YES, WORDWRAP=YES, FONT=\"Courier, 9\","
			"APPENDNEWLINE=NO, VALUE=\"Welcome to Pic2MCMap! Select an image or map to open through the File menu.\n\"");

	list = IupList(NULL);
	IupSetAttributes(list, "EXPAND=VERTICAL, SHOWIMAGE=YES, MINSIZE=155x");

	sizer = IupSplit(IupHbox(list,NULL), console);
	IupSetAttributes(sizer, "ORIENTATION=HORIZONTAL, VALUE=1000");
	vbox = IupVbox(sizer,NULL);

	//Create the window, put everything inside, and show it
	win = IupDialog(vbox);
	IupSetAttributes(win,"TITLE=\"Pic2MCMap - Picture to Minecraft map format converter\", MINSIZE=800x600");
	IupSetAttributeHandle(win,"MENU",menu);
	IupSetCallback(win,"K_ANY",k_any);
	IupShow(win);
}

Ihandle* create_submenu(const char* label, MenuItem* items)
{
	Ihandle* m = IupMenu(NULL);

	int i;

	for(i = 0; items[i].label ; i++)
	{
		if(items[i].label == SEPARATOR)
			IupAppend(m,IupSeparator());
		else
		{
			Ihandle* e = IupItem(items[i].label,NULL);
			IupSetCallback(e,"ACTION",items[i].cb);
			IupAppend(m,e);
		}
	}

	return IupSubmenu(label,m);
}

void log_console(const char* msg)
{
	IupSetAttribute(console,"APPEND",msg);
}

void add_image(imImage* addr)
{
	IupSetAttribute(list, "APPENDITEM", "");
	int count = IupGetInt(list, "COUNT");
	char id[10];
	sprintf(id, "IMAGE%d", count);
	Ihandle* img = IupImageFromImImage(addr);
	IupSetAttributeHandle(list, id, img);
}
