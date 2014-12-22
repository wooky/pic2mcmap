#ifndef READIMAGE_H_
#define READIMAGE_H_

#include <iup.h>
#include <im.h>
#include <im_image.h>

int open_image_file(Ihandle*);
void parse_image_file(Ihandle*, const char*);
imImage* get_image_thumbnail(const imImage*);
imImage** split_to_grid(const imImage*, unsigned char*, unsigned char*, unsigned char**);
Ihandle** grid_images(const imImage**, int);

#endif /* READIMAGE_H_ */
