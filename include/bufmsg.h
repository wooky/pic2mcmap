#ifndef HEADER_BUFMSG_H_
#define HEADER_BUFMSG_H_

#include <iup.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void buf_msg_init(const char* title, const char* description);
void buf_msg_put(char*);
void buf_msg_show();
void show_warning(const char* title, const char* message);

#endif /* HEADER_BUFMSG_H_ */
