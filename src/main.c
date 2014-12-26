#include "header/mainwindow.h"

#include <iup.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

Ihandle* win = NULL;

void on_crash()
{
	Ihandle* dead = IupMessageDlg();
	IupSetAttributeHandle(dead, "PARENTDIALOG", win);
	IupSetAttributes(dead, "TITLE=\"Pic2MCMap has Crashed!\", VALUE=\"It appears the program has crashed.\nThis probably happened because you tried to open "
			"an image it didn't like.\nWould you like to submit an issue to the project's GitHub page?\", BUTTONS=YESNO, DIALOGTYPE=ERROR");
	IupPopup(dead, IUP_CURRENT, IUP_CURRENT);
	if(*IupGetAttribute(dead, "BUTTONRESPONSE") == '1')
#define ISSUES_PAGE "https://github.com/wooky/pic2mcmap/issues"
#ifdef WIN32
		system("start "ISSUES_PAGE);
#else
		system("firefox "ISSUES_PAGE);	//TODO: really bad
#endif

	abort();
}

int main(int argc, char** argv)
{
	//Really crappy error handler
	signal(SIGSEGV, on_crash);

	//Initialize IUP
	if(IupOpen(NULL,NULL) == IUP_ERROR)
	{
		printf("Error initializing IUP! Make sure the graphical environment is running.\n");
		return 1;
	}

	//Create the main window
	win = create_mainwindow(argc, argv);

	//Mainloop
	IupMainLoop();

	//Clean up and exit
	cleanup();
	IupClose();
	return 0;
}
