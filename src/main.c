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

	Ihandle* win = IupDialog(NULL);
	IupSetAttribute(win,"SIZE","QUARTER");
	IupShow(win);

	//Mainloop and exit
	IupMainLoop();
	IupClose();
	return 0;
}
