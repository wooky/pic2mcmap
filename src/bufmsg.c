#include "bufmsg.h"

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

void buf_msg_show()
{
	if(done)
		return;

	if(triggered)
		show_warning(ttl, buf);

	free(ttl);
	free(buf);
	triggered = 0;
	done = 1;
}

void show_warning(const char* title, const char* message)
{
	Ihandle* dlg = IupMessageDlg();
	IupSetAttribute(dlg, "DIALOGTYPE", "WARNING");
	IupSetAttribute(dlg, "TITLE", title);
	IupSetAttribute(dlg, "VALUE", message);
	IupPopup(dlg, IUP_CURRENT, IUP_CURRENT);
	IupDestroy(dlg);
}
