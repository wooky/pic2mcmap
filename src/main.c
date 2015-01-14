#include "header/mainwindow.h"
#include "header/datformat.hpp"

#include <iup.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

Ihandle* win = NULL;

static void _on_crash()
{
	Ihandle* dead = IupMessageDlg();
	IupSetAttributeHandle(dead, "PARENTDIALOG", win);
	IupSetAttributes(dead, "TITLE=\"Pic2MCMap has Crashed!\", VALUE=\"It appears the program has crashed.\nThis probably happened because you tried to open "
			"an image it didn't like.\nWould you like to submit an issue to the project's GitHub page?\", BUTTONS=YESNO, DIALOGTYPE=ERROR");
	IupPopup(dead, IUP_CURRENT, IUP_CURRENT);
	if(*IupGetAttribute(dead, "BUTTONRESPONSE") == '1')
		IupHelp("https://github.com/wooky/pic2mcmap/issues");

	exit(-1);
}

int main(int argc, char** argv)
{
	//Really crappy error handler
	signal(SIGSEGV, _on_crash);

	//Initialize IUP
	if(IupOpen(NULL,NULL) == IUP_ERROR)
	{
		printf("Error initializing IUP! Make sure the graphical environment is running.\n");
		return 1;
	}

	//Initialize the DAT format driver
	formatRegisterDAT();

	//Create the main window
	win = main_window_create(argc, argv);

	//Mainloop
	IupMainLoop();

	//Clean up and exit
	main_window_cleanup();
	IupClose();
	return 0;
}
