#ifndef HEADER_IMAGEUTIL_H_
#define HEADER_IMAGEUTIL_H_

#include <iup.h>
#include <im.h>
#include <im_image.h>

imImage* util_get_thumbnail(imImage*);
imImage** util_split_to_grid(imImage*, unsigned char*, unsigned char*);
Ihandle** util_grid_images(imImage**, int);

const char* imIupErrorMessage(int);

#endif /* HEADER_IMAGEUTIL_H_ */
