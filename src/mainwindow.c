#include "header/mainwindow.h"
#include "header/imageio.h"
#include "header/linkedlist.h"
#include "header/statusbar.h"
#include "header/bufmsg.h"
#include "header/exportdialog.h"

#include <iup.h>
#include <im_image.h>
#include <im_process_loc.h>
#include <iupim.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

static Ihandle* placeholder;	//This is needed for some reason
static Ihandle* preview;		//Previewed image
static Ihandle* imgmod;			//Modified image (grid)

//Keyboard shortcuts. Format: {shortcut, callback}
static Keyboard keyboard[] = {
	{iup_XkeyCtrl(K_O),					&open_image_file},
	{iup_XkeyCtrl(K_W),					&close_image},
	{iup_XkeyCtrl(iup_XkeyShift(K_W)),	&close_all},
	{iup_XkeyCtrl(K_S),					&save_file},
	{iup_XkeyCtrl(iup_XkeyShift(K_E)),	&export_dialog_folder},
	{K_F1,								&wiki},
	{(int)NULL}
};

//Menus that might be modified
static Ihandle *menus_populated[5], *menus_selected[10];
static int nPopulated = 0, nSelected = 0;

void set_menu_state()
{
	int i;
	char* isPopulated = IupGetInt(list, "COUNT") ? "YES" : "NO";
	char* isSelected = IupGetInt(list, "VALUE") ? "YES" : "NO";

	for(i = 0; i < nPopulated; i++)
		IupSetAttribute(menus_populated[i], "ACTIVE", isPopulated);

	for(i = 0; i < nSelected; i++)
		IupSetAttribute(menus_selected[i], "ACTIVE", isSelected);
}

int k_any(Ihandle* ih, int c)
{
	int i;
	for(i = 0; keyboard[i].key; i++)
	{
		if(c == keyboard[i].key)
		{
			if(keyboard[i].cb != NULL)	//TEMPORARY, all keyboard shortcuts will be mapped eventually
			{
				return (keyboard[i].cb)();
			}
		}
	}

	return IUP_DEFAULT;
}

int render_image(Ihandle *ih, char *text, int item, int state)
{
	if(state == 1)
	{
		LinkedList *ll = LL_get(images, item-1);
		if(ll == NULL || ll->contents == NULL)
			return IUP_ERROR;
		imImage* img = ll->contents;

		//Display the scaled image
		char buf[64];
		sprintf(buf,"%dx%d",img->width,img->height);
		IupSetAttribute(preview, "RASTERSIZE", buf);
		IupSetAttributeHandle(preview, "IMAGE", ll->iContents);
		IupRefresh(preview);

		//Recreate the grid where to put all the images
		Ihandle* child;
		while((child = IupGetChild(imgmod, 0)) != NULL)
			IupDestroy(child);
		IupSetInt(imgmod, "NUMDIV", ll->cols);

		//Put the images into the grid
		int i;
		for(i = 0; i < ll->cols * ll->rows; i++)
		{
			Ihandle* tinyimg = IupLabel(NULL);
			IupSetAttributeHandle(tinyimg, "IMAGE", ll->iGrid[i]);
			IupAppend(imgmod, tinyimg);
			IupMap(tinyimg);
		}
		IupRefresh(imgmod);
	}

	set_menu_state();
	return IUP_DEFAULT;
}

Ihandle* create_mainwindow(int argc, char** argv)
{
	//Create the menu
	Ihandle* menu = IupMenu(
		create_submenu("&File",(MenuItem[]){
			{"&Open...\tCtrl+O", (Icallback)open_image_file, CONDITION_NONE},
			{"&Close\tCtrl+W", (Icallback)close_image, CONDITION_SELECTED},
			{"C&lose All\tCtrl+Shift+W", (Icallback)close_all, CONDITION_POPULATED},
			{SEPARATOR},
			{"&Save Map Matrix As Image...\tCtrl+S", (Icallback)save_file, CONDITION_SELECTED},
			{SEPARATOR},
			{"E&xit", (Icallback)IupExitLoop, CONDITION_NONE},
			{NULL}
		}),
		create_submenu("&Map",(MenuItem[]){
			{"&Import from World...\tCtrl+I", NULL, CONDITION_NONE},
			{"I&mport as Matrix...\tCtrl+Shift+I", NULL, CONDITION_NONE},
			{SEPARATOR},
			{"&Export to World...\tCtrl+E", NULL, CONDITION_SELECTED},
			{"E&xport as Matrix...\tCtrl+Shift+E", (Icallback)export_dialog_folder, CONDITION_SELECTED},
			{NULL}
		}),
		create_submenu("&Tools",(MenuItem[]){
			{"&Options", NULL, CONDITION_NONE},
			{NULL}
		}),
		create_submenu("&Help",(MenuItem[]){
			{"&Wiki\tF1", (Icallback)wiki, CONDITION_NONE},
			{"&About", (Icallback)about, CONDITION_NONE},
			{NULL}
		}),
		NULL
	);

	//List of images
	list = IupList(NULL);
	IupSetAttributes(list, "EXPAND=VERTICAL, SHOWIMAGE=YES, MINSIZE=155x, VISIBLELINES=1");
	IupSetCallback(list, "ACTION", (Icallback)render_image);

	//Image containers
	placeholder = IupImage(1,1,0);	//This is needed for some reason
	preview = IupLabel(NULL);
	IupSetAttributeHandle(preview, "IMAGE", placeholder);
	imgmod = IupGridBox(NULL);
	IupSetAttributes(imgmod, "GAPLIN=5, GAPCOL=5");

	//Horizontal box to contain all but the console
	Ihandle* hbox = IupHbox(
		list,
		IupSplit(
			IupVbox(
					IupSetAttributes(IupLabel("Scaled Image"),"ALIGNMENT=ACENTER, EXPAND=HORIZONTAL"),
					IupScrollBox(preview),
					NULL
			),
			IupVbox(
					IupSetAttributes(IupLabel("Resulting Map"),"ALIGNMENT=ACENTER, EXPAND=HORIZONTAL"),
					IupScrollBox(imgmod),
					NULL
			)
		),
		NULL
	);

	//Status bar
	Ihandle* statusbar = status_bar_setup();

	Ihandle* vbox = IupVbox(hbox, statusbar, NULL);

	//Create the window, put everything inside, and show it
	win = IupDialog(vbox);
	IupSetAttributes(win,"TITLE=\"Pic2MCMap - Picture to Minecraft map format converter\", MINSIZE=800x600");
	IupSetAttributeHandle(win, "MENU",menu);
	IupSetCallback(win, "K_ANY", (Icallback)k_any);
	IupSetCallback(win, "DROPFILES_CB", (Icallback)parse_image_file);
	IupShow(win);

	//Now try to open a few files given by the command line
	int i;
	for(i = 1; i < argc; i++)
		parse_image_file(NULL, argv[i], argc-i-1, 0, 0);

	return win;
}

//Clean up the program by deleting any variables saved in heap
void cleanup()
{
	IupDestroy(placeholder);
	IupDestroy(win);
	LL_purge(&images);
}

//Create a submenu
//Pass the name of the menu and an array of MenuItems, the last of which must be {NULL}
Ihandle* create_submenu(const char* label, MenuItem* items)
{
	Ihandle* m = IupMenu(NULL);

	int i;

	for(i = 0; items[i].label ; i++)
	{
		if(items[i].label == SEPARATOR)
			IupAppend(m, IupSeparator());
		else
		{
			Ihandle* e = IupItem(items[i].label, NULL);
			IupSetCallback(e, "ACTION", items[i].cb);
			switch(items[i].condition)
			{
			case CONDITION_NONE:
				break;
			case CONDITION_POPULATED:
				IupSetAttribute(e, "ACTIVE", "NO");
				menus_populated[nPopulated++] = e;
				break;
			case CONDITION_SELECTED:
				IupSetAttribute(e, "ACTIVE", "NO");
				menus_selected[nSelected++] = e;
				break;
			}
			IupAppend(m,e);
		}
	}

	return IupSubmenu(label,m);
}

void add_image(imImage* addr)
{
	LinkedList* ll = LL_insert(&images, addr, LL_APPEND);

	IupSetAttribute(list, "APPENDITEM", "");
	int count = IupGetInt(list, "COUNT");
	char id[10];
	sprintf(id, "IMAGE%d", count);
	IupSetAttributeHandle(list, id, ll->iThumbnail);
}

//Open up the wiki page
int wiki(Ihandle* self)
{
#define WIKI_URL "https://github.com/wooky/pic2mcmap/wiki"
	IupHelp(WIKI_URL);
	return IUP_DEFAULT;
}

//Display the about page
int about(Ihandle* self)
{
#define C "\xa9"
#define VERSION "0.1 alpha"
	IupMessage("About",
			"Pic2MCMap Minecraft map format converter\n"
			"Version " VERSION "\n"
			"Built on " __DATE__ "\n"
			"Copyright "C"2014-2015 Yakov Lipkovich\n"
			"Licensed under the BSD license\n"
			"---------------------------------------------------------------\n"
			"Additional libraries:\n"
			" * IUP Portable User Interface; "C"1994-2014 Tecgraf, PUC-Rio\n"
			" * IM Digital Imaging; "C"1994-2014 Tecgraf, PUC-Rio\n"
			" * cNBT NBT File Parser; "C"Lukas Niederbremer and Clark Gaebel\n"
			"---------------------------------------------------------------\n"
			"github.com/wooky/pic2mcmap"
	);
#undef C
#undef VERSION

	return IUP_DEFAULT;
}

int close_image(Ihandle* self)
{
	//Return if we have no images
	if(!IupGetInt(list, "COUNT"))
		return IUP_DEFAULT;

	//Remove image from the preview
	int index = IupGetInt(list, "VALUE") - 1;
	IupSetAttribute(list, "REMOVEITEM", IupGetAttribute(list, "VALUE"));

	//Set the images displayed to some placeholders (or just wipe everything out)
	IupSetAttributeHandle(preview, "IMAGE", placeholder);
	Ihandle* child;
	while((child = IupGetChild(imgmod, 0)) != NULL)
		IupDestroy(child);

	//Delete the linked list element associated with the image being deleted
	LL_remove(&images, index);

	set_menu_state();
	return IUP_DEFAULT;
}

int close_all(Ihandle* self)
{
	//Return if we have no images
	if(!IupGetInt(list, "COUNT"))
		return IUP_DEFAULT;

	//Set the images displayed to some placeholders (or just wipe everything out)
		IupSetAttributeHandle(preview, "IMAGE", placeholder);
		Ihandle* child;
		while((child = IupGetChild(imgmod, 0)) != NULL)
			IupDestroy(child);

	//Remove all images from the preview
	IupSetAttribute(list, "REMOVEITEM", NULL);

	//Purge the linked list
	LL_purge(&images);

	set_menu_state();
	return IUP_DEFAULT;
}

