#ifndef MAINWINDOW_H_LEL
#define MAINWINDOW_H_LEL

#include <iup.h>

#define SEPARATOR (char*)1
#define SHIFT 1
#define CONTROL 2

Ihandle* win;

typedef struct {
	const char* label;
	Icallback cb;
} MenuItem;

typedef int (*FunctionCallback)();
typedef struct {
	int key;
	FunctionCallback cb;
} Keyboard;

int test(Ihandle*); //pls remove
int k_all(Ihandle*,int);
void create_mainwindow();
Ihandle* create_submenu(const char*, MenuItem*);

#endif /* MAINWINDOW_H_LEL */
