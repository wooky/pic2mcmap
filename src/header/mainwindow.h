#ifndef MAINWINDOW_H_LEL
#define MAINWINDOW_H_LEL

#include <iup.h>

#define SEPARATOR 1

typedef struct {
	const char* label;
	Icallback cb;
	const char key;
} MenuItem;

void create_mainwindow();
Ihandle* create_submenu(const char*, MenuItem*);

#endif /* MAINWINDOW_H_LEL */
