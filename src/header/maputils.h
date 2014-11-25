#ifndef SRC_HEADER_MAPUTILS_H_
#define SRC_HEADER_MAPUTILS_H_

#include <im.h>
#include <im_image.h>

int nearest_color_index(unsigned char, unsigned char, unsigned char);
imImage* mapify(imImage*);

#endif /* SRC_HEADER_MAPUTILS_H_ */
