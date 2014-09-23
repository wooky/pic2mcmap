#ifndef READIMAGE_H_
#define READIMAGE_H_

#include <iup.h>

int open_image_file(Ihandle*);
int open_image_folder(Ihandle*);
void out_of_memory(Ihandle*);
void cleanup();

#endif /* READIMAGE_H_ */
