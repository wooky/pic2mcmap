#ifndef MAINWINDOW_H_LEL
#define MAINWINDOW_H_LEL

#include "imageio.h"
#include "linkedlist.h"
#include "statusbar.h"
#include "bufmsg.h"
#include "exportdialog.h"

#include <iup.h>
#include <im_image.h>
#include <im_process_loc.h>
#include <iupim.h>

#include <stdio.h>
#include <stdlib.h>

Ihandle* main_window;

//Path separator
#ifdef _WIN32
#define PATHSEP "\\"
#else
#define PATHSEP "/"
#endif

void main_window_set_menu_state();
void main_window_cleanup();
void main_window_add_image(imImage*);
LinkedList* main_window_get_images_if_populated();

Ihandle* main_window_create(int, char**);

#endif /* MAINWINDOW_H_LEL */
