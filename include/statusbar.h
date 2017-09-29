#ifndef HEADER_STATUSBAR_H_
#define HEADER_STATUSBAR_H_

#include <iup.h>

#include <stdio.h>

Ihandle* status_bar_setup();
void status_bar_init(const char*);
void status_bar_count(int);
void status_bar_inc();
void status_bar_done();

#endif /* HEADER_STATUSBAR_H_ */
