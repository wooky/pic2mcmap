#ifndef READIMAGE_H_
#define READIMAGE_H_

#include <iup.h>
#include <im.h>
#include <im_image.h>

int image_open_file(Ihandle*);
void image_parse_file(Ihandle*, const char*, int, int, int);

int image_save_file(Ihandle* ih);

#endif /* READIMAGE_H_ */
