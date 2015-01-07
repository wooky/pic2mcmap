#ifndef READIMAGE_H_
#define READIMAGE_H_

#include <iup.h>
#include <im.h>
#include <im_image.h>

int open_image_file(Ihandle*);
void parse_image_file(Ihandle*, const char*, int, int, int);

int save_file(Ihandle* ih);

#endif /* READIMAGE_H_ */
