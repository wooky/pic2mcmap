#ifndef HEADER_IMAGEUTIL_H_
#define HEADER_IMAGEUTIL_H_

#include <iup.h>
#include <im.h>
#include <im_image.h>

imImage* get_image_thumbnail(imImage*);
imImage** split_to_grid(imImage*, unsigned char*, unsigned char*);
Ihandle** grid_images(imImage**, int);

const char* imIupErrorMessage(int);

#endif /* HEADER_IMAGEUTIL_H_ */
