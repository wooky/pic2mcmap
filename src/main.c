#include "header/mainwindow.h"

#include <iup.h>
#include <stdio.h>

int main()
{
	//Initialize IUP
	if(IupOpen(NULL,NULL) == IUP_ERROR)
	{
		printf("Error initializing IUP!\n");
		return 1;
	}

	//Create the main window
	create_mainwindow();

	//Mainloop and exit
	IupMainLoop();
	IupClose();
	return 0;
}
