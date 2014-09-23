#include "header/mainwindow.h"
#include "header/readimage.h"

#include <iup.h>
#include <stddef.h>

//Keyboard shortcuts. Format: {shortcut, modifiers, callback}
Keyboard keyboard[] = {
	{iup_XkeyCtrl(K_O),&open_image_file},
	{iup_XkeyCtrl(K_I),&test},
	{iup_XkeyCtrl(K_W),NULL},
	{iup_XkeyCtrl(iup_XkeyShift(K_W)),NULL},
	{iup_XkeyCtrl(K_S),NULL},
	{iup_XkeyCtrl(iup_XkeyShift(K_S)),NULL},
	{NULL}
};

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
			if(keyboard[i].cb != NULL)	//TEMPORARY
			{
				return (Icallback)(keyboard[i].cb)();
			}
		}
	}

	return (Icallback)IUP_DEFAULT;
}

void create_mainwindow()
{
	Ihandle *menu, *win;

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

	//Create the menu
	menu = IupMenu(
		create_submenu("&File",fmenu),
		NULL
	);

	//Create the window, put everything inside, and show it
	win = IupDialog(NULL);
	IupSetAttribute(win,"TITLE","Pic2MCMap - Picture to Minecraft map format converter");
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
