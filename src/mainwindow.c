#include "header/mainwindow.h"

#define SEPARATOR (char*)1

//MenuItem struct for easy construction of menu items
typedef enum {CONDITION_NONE, CONDITION_POPULATED, CONDITION_SELECTED} ActivationCondition;
typedef struct {
	const char* label;
	Icallback cb;
	ActivationCondition condition;
} MenuItem;

//Keyboard struct for easy declaration of keyboard shortcuts
typedef int (*FunctionCallback)();
typedef struct {
	int key;
	FunctionCallback cb;
} Keyboard;

//GUI elements used in multiple functions
static Ihandle* placeholder;	//This is needed for some reason
static Ihandle* preview;		//Previewed image
static Ihandle* imgmod;			//Modified image (grid)
static Ihandle* list;			//List of images

//The linked list containing images
static LinkedList* images;

//Menus that might be modified
static Ihandle *menus_populated[5], *menus_selected[10];
static int nPopulated = 0, nSelected = 0;

///////////////////////////////////////HELPER FUNCTIONS////////////////////////////////////////////////

static int _render_image(Ihandle *ih, char *text, int item, int state)
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

	main_window_set_menu_state();
	return IUP_DEFAULT;
}

//Pass the name of the menu and an array of MenuItems, the last of which must be {NULL}
Ihandle* _create_submenu(const char* label, MenuItem* items)
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

//////////////////////////////////////////////FUNCTION CALLBACKS///////////////////////////////////

//Open up the wiki page
int _wiki(Ihandle* self)
{
#define WIKI_URL "https://github.com/wooky/pic2mcmap/wiki"
	IupHelp(WIKI_URL);
	return IUP_DEFAULT;
}

//Display the about page
int _about(Ihandle* self)
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

int _close_image(Ihandle* self)
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

	main_window_set_menu_state();
	return IUP_DEFAULT;
}

int _close_all(Ihandle* self)
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

	main_window_set_menu_state();
	return IUP_DEFAULT;
}

//////////////////////////////////////////////WINDOW CALLBACKS//////////////////////////////////

//Keyboard shortcuts. Format: {shortcut, callback}
static Keyboard keyboard[] = {
	{iup_XkeyCtrl(K_O),					&image_open_file},
	{iup_XkeyCtrl(K_W),					&_close_image},
	{iup_XkeyCtrl(iup_XkeyShift(K_W)),	&_close_all},
	{iup_XkeyCtrl(K_S),					&image_save_file},
	{iup_XkeyCtrl(iup_XkeyShift(K_E)),	&export_dialog_folder},
	{K_F1,								&_wiki},
	{(int)NULL}
};

int _k_any(Ihandle* ih, int c)
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

////////////////////////////////////////GLOBAL FUNCTIONS////////////////////////////////////////

void main_window_set_menu_state()
{
	int i;
	char* isPopulated = IupGetInt(list, "COUNT") ? "YES" : "NO";
	char* isSelected = IupGetInt(list, "VALUE") ? "YES" : "NO";

	for(i = 0; i < nPopulated; i++)
		IupSetAttribute(menus_populated[i], "ACTIVE", isPopulated);

	for(i = 0; i < nSelected; i++)
		IupSetAttribute(menus_selected[i], "ACTIVE", isSelected);
}

//Clean up the program by deleting any variables saved in heap
void main_window_cleanup()
{
	IupDestroy(placeholder);
	IupDestroy(main_window);
	LL_purge(&images);
}

void main_window_add_image(imImage* addr)
{
	LinkedList* ll = LL_insert(&images, addr, LL_APPEND);

	IupSetAttribute(list, "APPENDITEM", "");
	int count = IupGetInt(list, "COUNT");
	char id[10];
	sprintf(id, "IMAGE%d", count);
	IupSetAttributeHandle(list, id, ll->iThumbnail);
}

LinkedList* main_window_get_images_if_populated()
{
	if(!IupGetInt(list, "COUNT"))
		return NULL;

	int val = IupGetInt(list,"VALUE");
	if(val <= 0)
		return NULL;

	//Could very well return NULL
	return LL_get(images, val-1);
}

//////////////////////////////////MAIN WINDOW CONSTRUCTOR/////////////////////////////////////
Ihandle* main_window_create(int argc, char** argv)
{
	//Create the menu
	Ihandle* menu = IupMenu(
		_create_submenu("&File",(MenuItem[]){
			{"&Open...\tCtrl+O", (Icallback)image_open_file, CONDITION_NONE},
			{"&Close\tCtrl+W", (Icallback)_close_image, CONDITION_SELECTED},
			{"C&lose All\tCtrl+Shift+W", (Icallback)_close_all, CONDITION_POPULATED},
			{SEPARATOR},
			{"&Save Map Matrix As Image...\tCtrl+S", (Icallback)image_save_file, CONDITION_SELECTED},
			{SEPARATOR},
			{"E&xit", (Icallback)IupExitLoop, CONDITION_NONE},
			{NULL}
		}),
		_create_submenu("&Map",(MenuItem[]){
			{"&Import from World...\tCtrl+I", NULL, CONDITION_NONE},
			{"I&mport as Matrix...\tCtrl+Shift+I", NULL, CONDITION_NONE},
			{SEPARATOR},
			{"&Export to World...\tCtrl+E", NULL, CONDITION_SELECTED},
			{"E&xport as Matrix...\tCtrl+Shift+E", (Icallback)export_dialog_folder, CONDITION_SELECTED},
			{NULL}
		}),
		_create_submenu("&Tools",(MenuItem[]){
			{"&Options", NULL, CONDITION_NONE},
			{NULL}
		}),
		_create_submenu("&Help",(MenuItem[]){
			{"&Wiki\tF1", (Icallback)_wiki, CONDITION_NONE},
			{"&About", (Icallback)_about, CONDITION_NONE},
			{NULL}
		}),
		NULL
	);

	//List of images
	list = IupList(NULL);
	IupSetAttributes(list, "EXPAND=VERTICAL, SHOWIMAGE=YES, MINSIZE=155x, VISIBLELINES=1");
	IupSetCallback(list, "ACTION", (Icallback)_render_image);

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
	main_window = IupDialog(vbox);
	IupSetAttributes(main_window,"TITLE=\"Pic2MCMap - Picture to Minecraft map format converter\", MINSIZE=800x600");
	IupSetAttributeHandle(main_window, "MENU",menu);
	IupSetCallback(main_window, "K_ANY", (Icallback)_k_any);
	IupSetCallback(main_window, "DROPFILES_CB", (Icallback)image_parse_file);
	IupShow(main_window);

	//Now try to open a few files given by the command line
	int i;
	for(i = 1; i < argc; i++)
		image_parse_file(NULL, argv[i], argc-i-1, 0, 0);

	return main_window;
}
