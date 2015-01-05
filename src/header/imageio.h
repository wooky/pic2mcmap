#ifndef READIMAGE_H_
#define READIMAGE_H_

#include <iup.h>
#include <im.h>
#include <im_image.h>

int open_image_file(Ihandle*);
void parse_image_file(Ihandle*, const char*, int, int, int);
imImage* get_image_thumbnail(imImage*);
imImage** split_to_grid(imImage*, unsigned char*, unsigned char*);
Ihandle** grid_images(imImage**, int);

int save_file(Ihandle* ih);

const char* imIupErrorMessage(int);

#endif /* READIMAGE_H_ */
