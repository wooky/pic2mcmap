#ifndef MAINWINDOW_H_LEL
#define MAINWINDOW_H_LEL

#include "linkedlist.h"

#include <iup.h>
#include <im.h>
#include <im_image.h>

#include <stddef.h>

#define SEPARATOR (char*)1
#define SHIFT 1
#define CONTROL 2

Ihandle* win;
Ihandle* list;		//List of images opened
LinkedList *images;	//Size can be accessed by IupGetInt(list,"COUNT")

typedef struct {
	const char* label;
	Icallback cb;
	char disabled;
} MenuItem;

typedef int (*FunctionCallback)();
typedef struct {
	int key;
	FunctionCallback cb;
} Keyboard;

int wiki(Ihandle*);
Icallback k_all(Ihandle*, int);
int render_image(Ihandle*, char*, int, int);
Ihandle* create_mainwindow(int, char**);
void cleanup();
Ihandle* create_submenu(const char*, MenuItem*);
void log_console(const char*);
void add_image(imImage*);
int close_image(Ihandle*);
int close_all(Ihandle*);
int save_nbt(Ihandle*);
int export_image(Ihandle*);

#endif /* MAINWINDOW_H_LEL */
