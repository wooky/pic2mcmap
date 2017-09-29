#include "imageutil.h"

//Put these into the Options dialog, not hard-coded
#define RESIZE_ORDER 0

//This is the palette used by the Minecraft maps
//Retrieved from http://minecraft.gamepedia.com/Map_item_format#Map_colors
//Converted into arrays of {r,g,b}
#define DATPALETTE_SIZE 144
#define TRANSPARENT {255,0,255}	//Invalid pattern just cause
static const unsigned char DATPalette[DATPALETTE_SIZE][3] = {
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
//We're using this function rather than the one given by IM because that one sucks for some reason
#define square(x) (x)*(x)
static unsigned char _nearest_color_index(unsigned char r, unsigned char g, unsigned char b)
{
	short index = -1, i;
	unsigned int diff = -1;

	for(i = 4; i < DATPALETTE_SIZE; i++)
	{
		unsigned int this_diff = square(r-DATPalette[i][0]) + square(g-DATPalette[i][1]) + square(b-DATPalette[i][2]);
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

//I would like to use imConvertColorSpace instead, however that function doesn't handle transparencies, which is quite critical for this program.
//Thus, I have to write my own function.
static imImage* _mapify(imImage* orig)
{
	//If the original file is already a DAT file, just dupe the given image and return it
	if(imImageGetAttribute(orig, "DAT", NULL, NULL))
		return imImageDuplicate(orig);

	//Get the original image's width and height, anything else will be modified
	imImage* mod = imImageCreateBased(orig, -1, -1, IM_MAP | IM_TOPDOWN, IM_BYTE);

	//Set the palette
	mod->palette = DATPaletteMap;

	//Set the transparent color
	imImageSetAttribInteger(mod, "TransparencyIndex", IM_BYTE, 0);

	//Is it a map? Is there an alpha layer? How many layers in total?
	char isMap = orig->color_space & IM_MAP;
	int alphaLayer = orig->depth;

	//Is there an alpha layer or index?
	short hasAlpha = orig->has_alpha;	//Can't be a char because has_alpha can be 256
	int transparencyIndexType = IM_BYTE, transparencyIndexSize = 1;
	const void* transparentIndexAddress = imImageGetAttribute(orig, "TransparencyIndex", &transparencyIndexType, &transparencyIndexSize);
	unsigned char transparencyIndex = transparentIndexAddress ? *((unsigned char*)(transparentIndexAddress)) : 0;

	//Get the closest index for each pixel and save it to the new image
	int i, ppp = orig->count;
	unsigned char *data = (unsigned char*)orig->data[0], *nova = (unsigned char*)mod->data[0];

	for(i = 0; i < ppp; i++)
	{
		//Get the alpha, and if it's low enough, put in a transparent color
		if((hasAlpha && data[i+ppp*alphaLayer] < 128) || (transparentIndexAddress && data[i] == transparencyIndex))
		{
			nova[i] = 0;
			continue;
		}

		unsigned char r,g,b;

		//If it's a map format, the colors are based on the palette
		if(isMap)
			imColorDecode(&r, &g, &b, orig->palette[data[i]]);
		//Otherwise, it's packed
		else
		{
			r = data[i];
			g = data[i+ppp];
			b = data[i+ppp*2];
		}

		//Get the pixel value and see if it's transparent
		unsigned char nearest_color = _nearest_color_index(r, g, b);
		nova[i] = nearest_color;
		if(!nearest_color)
			nova[ppp + i] = 255;
	}

	return mod;
}

imImage* util_get_thumbnail(imImage* orig)
{
	if(orig->width == 128 && orig->height == 128)
		return imImageDuplicate(orig);

	imImage *temp = imImageClone(orig);
	imImageReshape(temp, 128, 128);
	imProcessResize(orig, temp, RESIZE_ORDER);
	return temp;
}

imImage** util_split_to_grid(imImage* orig, unsigned short* rows, unsigned short* cols)
{
	unsigned short nCols = orig->width/128, nRows = orig->height/128, i,j;
	status_bar_count(nCols * nRows);

	*cols = nCols;
	*rows = nRows;
	imImage** matrix = malloc(nCols * nRows * sizeof(imImage*));
	imImage* temp = imImageCreateBased(orig, 128, 128, -1, -1);

	for(i = 0; i < nRows; i++)
	{
		for(j = 0; j < nCols; j++)
		{
			//                               When the image is top-down, crop the top; otherwise, from the "bottom" (which is actually the top)
			imProcessCrop(orig, temp, j*128, imColorModeIsTopDown(orig->color_space) ? i*128 : (nRows-i-1)*128);
			matrix[i*nCols + j] = _mapify(temp);

			status_bar_inc();
		}
	}
	imImageDestroy(temp);

	return matrix;
}

Ihandle** util_grid_images(imImage** matrix, int size)
{
	Ihandle** handle = malloc(size * sizeof(Ihandle*));
	int i;

	for(i = 0; i < size; i++)
		handle[i] = IupImageFromImImage(matrix[i]);

	return handle;
}

//Stolen almost directly from http://webserver2.tecgraf.puc-rio.br/im/en/storage_guide.html
const char* imIupErrorMessage(int error)
{
	char *msg;
	switch (error)
	{
	case IM_ERR_OPEN:
		msg = "Error Opening File.";
		break;
	case IM_ERR_MEM:
		msg = "Insufficient memory.";
		break;
	case IM_ERR_ACCESS:
		msg = "Error Accessing File.";
		break;
	case IM_ERR_DATA:
		msg = "Image type not supported.";
		break;
	case IM_ERR_FORMAT:
		msg = "Invalid Format.";
		break;
	case IM_ERR_COMPRESS:
		msg = "Invalid or unsupported compression.";
		break;
	default:
		msg = "Unknown Error.";
	}
	return msg;
}
