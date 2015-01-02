#ifndef MAINWINDOW_H_LEL
#define MAINWINDOW_H_LEL

#include "linkedlist.h"

#include <iup.h>
#include <im.h>
#include <im_image.h>

#include <stddef.h>

#define SEPARATOR (char*)1

Ihandle* win;		//The window
Ihandle* list;		//List of images opened
LinkedList *images;	//Size can be accessed by IupGetInt(list,"COUNT")

typedef enum {CONDITION_NONE, CONDITION_POPULATED, CONDITION_SELECTED} ActivationCondition;
typedef struct {
	const char* label;
	Icallback cb;
	ActivationCondition condition;
} MenuItem;

typedef int (*FunctionCallback)();
typedef struct {
	int key;
	FunctionCallback cb;
} Keyboard;

void set_menu_state();
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

#endif /* MAINWINDOW_H_LEL */
