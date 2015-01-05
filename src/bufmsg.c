#include "header/bufmsg.h"

#include <iup.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char done = 1, triggered = 0;
static char *ttl, *buf;

void buf_msg_init(const char* title, const char* description)
{
	if(!done)
		return;

	ttl = malloc((strlen(title)+1) * sizeof(char));
	if(!ttl)
		return;

	buf = malloc(16384 * sizeof(char));
	if(!buf)
	{
		free(ttl);
		return;
	}

	done = 0;
	strcpy(ttl, title);
	sprintf(buf, "%s\n", description);
}

void buf_msg_put(char* err)
{
	if(done)
		return;

	triggered = 1;
	sprintf(buf, "%s\n%s", buf, err);
}

void buf_msg_show(Ihandle* parent)
{
	if(done)
		return;

	if(triggered)
	{
		Ihandle* dlg = IupMessageDlg();
		IupSetAttribute(dlg, "DIALOGTYPE", "WARNING");
		IupSetAttributeHandle(dlg, "PARENTDIALOG", parent);
		IupSetAttribute(dlg, "TITLE", ttl);
		IupSetAttribute(dlg, "VALUE", buf);
		IupPopup(dlg, IUP_CURRENT, IUP_CURRENT);
		IupDestroy(dlg);
		triggered = 0;
	}

	free(ttl);
	free(buf);
	done = 1;
}
