#ifndef SRC_HEADER_MAPUTILS_H_
#define SRC_HEADER_MAPUTILS_H_

#include <im.h>
#include <im_image.h>

#define TRANSPARENT -1

int nearest_color_index(long);
imImage* mapify(imImage*);

#endif /* SRC_HEADER_MAPUTILS_H_ */
