#include "header/statusbar.h"

#include <iup.h>

#include <stdio.h>

unsigned char done = 1;

Ihandle *status_msg, *status_prog, *status_hbox;

//Set up the status bar to display on the main window
Ihandle* status_bar_setup()
{
	status_msg = IupLabel("Welcome to Pic2MCMap! Open an image or map to begin.");

	status_prog = IupProgressBar();
	IupSetAttribute(status_prog, "SIZE", "0x7");

	status_hbox = IupHbox(status_prog, status_msg);
	return status_hbox;
}

char status_bar_init(const char* description)
{
	//Return if not done
	//Hopefully will never happen
	if(!done)
		return 0;

	//We're using this now
	done = 0;

	//Set the message
	IupSetAttribute(status_msg, "TITLE", description);

	//Set the progress bar
	IupSetInt(status_prog, "VALUE", 0);
	IupSetAttribute(status_prog, "EXPAND", "HORIZONTAL");

	//Update
	IupRefresh(status_prog);
	IupLoopStep();

	return 1;
}

void status_bar_count(int iterations)
{
	if(done)
		return;

	IupSetInt(status_prog, "MAX", iterations);
	IupLoopStep();
}

void status_bar_inc()
{
	if(done)
		return;

	IupSetInt(status_prog, "VALUE", IupGetInt(status_prog, "VALUE")+1);
	IupLoopStep();
}

void status_bar_done()
{
	if(done)
		return;

	IupSetAttribute(status_msg, "TITLE", "Ready");
	IupSetAttribute(status_prog, "EXPAND", "NO");
	done = 1;

	IupRefresh(status_prog);
	IupLoopStep();
}
