#include "header/maputils.h"

#include <im.h>
#include <im_image.h>
#include <im_palette.h>

#include <stdio.h>

imImage* mapify(imImage* orig)
{
	//imImage *newimg = imImageInit(orig->width, orig->height, orig->color_space, orig->data_type, orig->data, mcpalette, orig->palette_count);
	imImage* newimg = imImageDuplicate(orig);
	int i,j;

	/*/Get the closest color for each pixel
	for(i = 0; i < newimg->width * newimg->height; i++)
	{
		newimg->data[0][i] = mcpalette[imPaletteFindNearest(mcpalette, PALETTE_SIZE, newimg->data[0][i])];
	}*/

	for(i = 0; i < newimg->depth; i++)
		for(j = 0; j < newimg->count; j++)
			printf("%ld ", (long)((long*)(newimg->data[i])[j]));

	imImageSetPalette(newimg, mcpalette, PALETTE_SIZE);

	return newimg;
}
