#include "header/maputils.h"

#include <im.h>
#include <im_image.h>
#include <im_convert.h>
#include <im_palette.h>
#include <im_util.h>

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

//This is the palette used by the Minecraft maps
//Retrieved from http://minecraft.gamepedia.com/Map_item_format#Map_colors
//Converted into long values by using the equation
//R*65536+G*256+B
#define PALETTE_SIZE 144
const long mcpalette[PALETTE_SIZE] = {
		TRANSPARENT, TRANSPARENT, TRANSPARENT, TRANSPARENT,
		5864743, 7182640, 8368696, 4415005,
		11445363, 14010764, 16247203, 8551254,
		7697781, 9474192, 10987431, 5789784,
		11796480, 14417920, 16711680, 8847360,
		7368884, 9079516, 10526975, 5526663,
		7697781, 9474192, 10987431, 5789784,
		22272, 27136, 31744, 16640,
		11842740, 14474460, 16777215, 8882055,
		7566977, 9277598, 10791096, 5658721,
		8473121, 10312488, 12020271, 6305816,
		5197647, 6316128, 7368816, 3881787,
		2960820, 3618780, 4210943, 2171271,
		4799011, 5850923, 6837042, 3615514,
		11841964, 14473683, 16776437, 8881537,
		9984292, 12217644, 14188339, 7488283,
		8205720, 10043834, 11685080, 6170738,
		4746392, 5801146, 6724056, 3559794,
		10592548, 12961068, 15066419, 7960859,
		5869585, 7188501, 8375321, 4418573,
		11164020, 13659534, 15892389, 8405847,
		3487029, 4276545, 5000268, 2631720,
		7105644, 8684676, 10066329, 5329233,
		3496300, 4287876, 5013401, 2638673,
		5844093, 7157401, 8339378, 4399454,
		2372989, 2900377, 3361970, 1779806,
		4732196, 5783852, 6704179, 3549211,
		4741412, 5795116, 6717235, 3556123,
		7087140, 8662060, 10040115, 5315355,
		1118481, 1381653, 1644825, 855309,
		11577398, 14142786, 16445005, 8683048,
		4233878, 5225655, 6085589, 3175280,
		3431092, 4157148, 4882687, 2573191,
		39208, 47922, 55610, 29214,
		921109, 1184026, 1381407, 723472,
		5177600, 6291712, 7340544, 3866880
};

//Get the nearest color index for the given color
int nearest_color_index(long color)
{
	int index = -1, i;
	long diff = LONG_MAX;

	for(i = 0; i < PALETTE_SIZE; i++)
	{
		long this_diff = labs(color - mcpalette[i]);
		if(this_diff == 0)
			return i;
		else if(this_diff < diff)
		{
			index = i;
			diff = this_diff;
		}
	}

	return index;
}

//Convert an image into a map representation (i.e. color degradation) and return a new image
imImage* mapify(imImage* orig)
{
	imImage* newimg = imImageDuplicate(orig);

	int i;

	/*/Get the closest color for each pixel
	for(i = 0; i < newimg->width * newimg->height; i++)
	{
		newimg->data[0][i] = mcpalette[imPaletteFindNearest(mcpalette, PALETTE_SIZE, newimg->data[0][i])];
	}*/

	unsigned char* data = (unsigned char*)newimg->data[0];
	int depth = newimg->depth;	//TODO: use this variable to account for alpha
	int ppp = newimg->count;
	for(i = 0; i < ppp; i ++)
	{
		long color = imColorEncode(data[i], data[ppp+i], data[ppp*2 + i]);
		int closest_index = nearest_color_index(color);
		if(i < 10) printf("%lx <%d> %lx\n",color,closest_index,mcpalette[closest_index]);
		imColorDecode(&data[i], &data[ppp+i], &data[ppp*2 + i], mcpalette[closest_index]);
	}

	return newimg;
}
