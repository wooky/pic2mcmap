#include "header/maputils.h"

#include <im.h>
#include <im_image.h>
#include <im_convert.h>
#include <im_palette.h>
#include <im_util.h>

#include <stdio.h>
#include <limits.h>

//This is the palette used by the Minecraft maps
//Retrieved from http://minecraft.gamepedia.com/Map_item_format#Map_colors
//Converted into arrays of {r,g,b}
#define PALETTE_SIZE 144
#define TRANSPARENT {0,0,0}	//Invalid pattern just cause
const unsigned char mcpalette[PALETTE_SIZE][3] = {
		TRANSPARENT, TRANSPARENT, TRANSPARENT, TRANSPARENT,
		{89,125,39},{109,153,48},{127,178,56},{67,94,29},
		{174,164,115},{213,201,140},{247,233,163},{130,123,86},
		{117,117,117},{144,144,144},{167,167,167},{88,88,88},
		{180,0,0},{220,0,0},{255,0,0},{135,0,0},
		{112,112,180},{138,138,220},{160,160,255},{84,84,135},
		{117,117,117},{144,144,144},{167,167,167},{88,88,88},
		{0,87,0},{0,106,0},{0,124,0},{0,65,0},
		{180,180,180},{220,220,220},{255,255,255},{135,135,135},
		{115,118,129},{141,144,158},{164,168,184},{86,88,97},
		{129,74,33},{157,91,40},{183,106,47},{96,56,24},
		{79,79,79},{96,96,96},{112,112,112},{59,59,59},
		{45,45,180},{55,55,220},{64,64,255},{33,33,135},
		{73,58,35},{89,71,43},{104,83,50},{55,43,26},
		{180,177,172},{220,217,211},{255,252,245},{135,133,129},
		{152,89,36},{186,109,44},{216,127,51},{114,67,27},
		{125,53,152},{153,65,186},{178,76,216},{94,40,114},
		{72,108,152},{88,132,186},{102,153,216},{54,81,114},
		{161,161,36},{197,197,44},{229,229,51},{121,121,27},
		{89,144,17},{109,176,21},{127,204,25},{67,108,13},
		{170,89,116},{208,109,142},{242,127,165},{128,67,87},
		{53,53,53},{65,65,65},{76,76,76},{40,40,40},
		{108,108,108},{132,132,132},{153,153,153},{81,81,81},
		{53,89,108},{65,109,132},{76,127,153},{40,67,81},
		{89,44,125},{109,54,153},{127,63,178},{67,33,94},
		{36,53,125},{44,65,153},{51,76,178},{27,40,94},
		{72,53,36},{88,65,44},{102,76,51},{54,40,27},
		{72,89,36},{88,109,44},{102,127,51},{54,67,27},
		{108,36,36},{132,44,44},{153,51,51},{81,27,27},
		{17,17,17},{21,21,21},{25,25,25},{13,13,13},
		{176,168,54},{215,205,66},{250,238,77},{132,126,40},
		{64,154,150},{79,188,183},{92,219,213},{48,115,112},
		{52,90,180},{63,110,220},{74,128,255},{39,67,135},
		{0,153,40},{0,187,50},{0,217,58},{0,114,30},
		{14,14,21},{18,17,26},{21,20,31},{11,10,16},
		{79,1,0},{96,1,0},{112,2,0},{59,1,0}
};

//Get the nearest color index for the given color
//Uses square Eucledian distance
#define square(x) (x)*(x)
unsigned char nearest_color_index(unsigned char r, unsigned char g, unsigned char b)
{
	short index = -1, i;
	int diff = INT_MAX;

	for(i = 4; i < PALETTE_SIZE; i++)
	{
		int this_diff = square(r-mcpalette[i][0]) + square(g-mcpalette[i][1]) + square(b-mcpalette[i][2]);
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
imImage* mapify(imImage* orig, unsigned char* index)
{
	imImage* newimg = imImageDuplicate(orig);

	int i;

	unsigned char* data = (unsigned char*)newimg->data[0];
	int depth = newimg->depth;	//TODO: use this variable to account for alpha
	int ppp = newimg->count;
	for(i = 0; i < ppp; i ++)
	{
		unsigned char closest_index = nearest_color_index(data[i], data[ppp+i], data[ppp*2 + i]);
		data[i] = mcpalette[closest_index][0];
		data[ppp+i] = mcpalette[closest_index][1];
		data[ppp*2 + i] = mcpalette[closest_index][2];
		index[i] = closest_index;
	}

	return newimg;
}

//Convert a map array to an image
imImage* unmapify(int width, int height, unsigned char* map)
{
	imImage* img = imImageCreate(width, height, IM_MAP, IM_BYTE);
	unsigned char* data = (unsigned char*)img->data[0];
	int size = width * height;

	int i;
	for(i = 0; i < size; i++)
	{
		data[i] = mcpalette[map[i]][0];
		data[size+i] = mcpalette[map[i]][1];
		data[size*2 + i] = mcpalette[map[i]][2];
	}

	return img;
}
