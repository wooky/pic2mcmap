#include "header/mainwindow.h"
#include "header/imageio.h"
#include "header/linkedlist.h"
#include "header/maputils.h"

#include <iup.h>
#include <im_image.h>
#include <im_process_loc.h>
#include <iupim.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

Ihandle* console;		//Logging console
Ihandle* list;			//List of images opened
Ihandle* placeholder;	//This is needed for some reason
Ihandle* preview;		//Previewed image
Ihandle* imgmod;		//Modified image (grid)

LinkedList *images = NULL;	//Size can be accessed by IupGetInt(list,"COUNT")

//Keyboard shortcuts. Format: {shortcut, callback}
Keyboard keyboard[] = {
	{iup_XkeyCtrl(K_O),&open_image_file},
	{iup_XkeyCtrl(K_I),NULL},
	{iup_XkeyCtrl(K_W),&close_image},
	{iup_XkeyCtrl(iup_XkeyShift(K_W)),&close_all},
	{iup_XkeyCtrl(K_S),NULL},
	{iup_XkeyCtrl(iup_XkeyShift(K_S)),NULL},
	{K_F1,&wiki},
	{(int)NULL}
};

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
			if(IupAppend(imgmod, tinyimg) == NULL)
			{
				sprintf(buf, "Failed to attach image piece %d\n", i);
				log_console(buf);
			}
			IupMap(tinyimg);
		}
		IupRefresh(imgmod);
	}
	return IUP_DEFAULT;
}

Ihandle* create_mainwindow(int argc, char** argv)
{
	//Create the menu
	Ihandle* menu = IupMenu(
		create_submenu("&File",(MenuItem[]){
			{"&Open...\tCtrl+O",(Icallback)open_image_file},
			{"&Close\tCtrl+W",(Icallback)close_image,1},
			{"C&lose All\tCtrl+Shift+W",(Icallback)close_all,1},
			{SEPARATOR},
			{"&Save to Map...\tCtrl+S",NULL,1},
			{"&Export to Image...\tCtrl+E",NULL,1},
			{SEPARATOR},
			{"E&xit", (Icallback)IupExitLoop},
			{NULL}
		}),
		create_submenu("&Map",(MenuItem[]){
			{"Import Map &Cluster...\tCtrl+Shift+I",NULL},
			{SEPARATOR},
			{"&Export to Map\tCtrl+S",NULL,1},
			{"Export to &World...\tCtrl+Alt+S",NULL,1},
			{NULL}
		}),
		create_submenu("&Tools",(MenuItem[]){
			{"&Options",NULL},
			{NULL}
		}),
		create_submenu("&Help",(MenuItem[]){
			{"&Wiki\tF1",(Icallback)wiki},
			{"&About",NULL},
			{NULL}
		}),
		NULL
	);

	//Status console
	console = IupText(NULL);
	IupSetAttributes(console,"MULTILINE=YES, READONLY=YES, MINSIZE=x85, EXPAND=YES, WORDWRAP=YES, FONT=\"Courier, 9\","
			"APPENDNEWLINE=NO, VALUE=\"Welcome to Pic2MCMap! Select an image or map to open through the File menu.\n\"");

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

	//Divider to resize the console
	Ihandle* sizer = IupSplit(hbox, console);
	IupSetAttributes(sizer, "ORIENTATION=HORIZONTAL, VALUE=1000");
	Ihandle* vbox = IupVbox(sizer,NULL);

	//Create the window, put everything inside, and show it
	win = IupDialog(vbox);
	IupSetAttributes(win,"TITLE=\"Pic2MCMap - Picture to Minecraft map format converter\", MINSIZE=800x600");
	IupSetAttributeHandle(win,"MENU",menu);
	IupSetCallback(win,"K_ANY",k_any);
	IupSetCallback(win, "DROPFILES_CB", parse_image_file);
	IupShow(win);

	//Now try to open a few files given by the command line
	int i;
	for(i = 1; i < argc; i++)
		parse_image_file(NULL, argv[i], 0, 0, 0);

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
			IupAppend(m,IupSeparator());
		else
		{
			Ihandle* e = IupItem(items[i].label,NULL);
			IupSetCallback(e,"ACTION",items[i].cb);
			if(items[i].disabled)
				IupSetAttribute(e,"ACTIVE","NO");
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
	LinkedList* ll = LL_insert(&images, addr, LL_APPEND);

	IupSetAttribute(list, "APPENDITEM", "");
	int count = IupGetInt(list, "COUNT");
	char id[10];
	sprintf(id, "IMAGE%d", count);
	IupSetAttributeHandle(list, id, ll->iThumbnail);
}

//Open up the wiki page
//NOTE! This is a platform specific implementation (unless I want to use CURL, which I don't)
//Probably won't work on OS X
int wiki(Ihandle* self)
{
#define WIKI_URL "https://github.com/wooky/pic2mcmap/wiki"
	IupHelp(WIKI_URL);
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

	return IUP_DEFAULT;
}

