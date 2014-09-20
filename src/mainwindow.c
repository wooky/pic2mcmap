#include "header/mainwindow.h"

#include <iup.h>
#include <stddef.h>

void create_mainwindow()
{
	Ihandle *menu, *win;

	//Aray of menus. Format: {name, callback}
	MenuItem fmenu[] = {
		{"&Open...\tCtrl+O",NULL},
		{"Open &Folder...\tCtrl+Shift+O",NULL},
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

	//Create the menu
	menu = IupMenu(
		create_submenu("&File",fmenu),
		NULL
	);

	//Create the window, put everything inside, and show it
	win = IupDialog(NULL);
	IupSetAttribute(win,"TITLE","Pic2MCMap - Picture to Minecraft map format converter");
	IupSetAttributeHandle(win,"MENU",menu);
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
			//if(items[i].key)
			//	IupSetAttribute(e,"KEY",items[i].key);
			IupAppend(m,e);
		}
	}

	return IupSubmenu(label,m);
}
