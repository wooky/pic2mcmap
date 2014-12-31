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

void save_file(const char* filter, const char* type, const char* extension);

#endif /* READIMAGE_H_ */
