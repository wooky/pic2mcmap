#include "header/mainwindow.h"
#include "header/datformat.hpp"

#include <iup.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

Ihandle* win = NULL;

static void _on_crash()
{
	int res = IupAlarm("Pic2MCMap has Crashed!", "Unfortunately, the program has crashed.\nIf this is the first time you see this message, trying repeating the same steps you made to get this "
			"message.\nIf this message reappears, please submit an issue to the project's GitHub page.\n\nThe program will now close.", "Open GitHub Page", "Exit", NULL);
	if(res == 1)
		IupHelp("https://github.com/wooky/pic2mcmap/issues");

	exit(-1);
}

int main(int argc, char** argv)
{
	//Error handler
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
