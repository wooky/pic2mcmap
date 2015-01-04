//Oh joy it's a C++ file in a C project. Nothing could possibly go wrong here
#include "header/datformat.hpp"

#include "header/nbt/nbt.h"
#include "header/nbttemplate.h"

#include <im_image.h>
#include <im_format.h>
#include <im_counter.h>
#include <im_palette.h>
#include <im_util.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define TRANSPARENT_MAP 0xFF00FF	//A color to represent transparency (i.e. eye-bleeding magenta)
#define DATPALETTE_SIZE_EFFECTIVE 140
long DATPaletteMap[256] = {
		TRANSPARENT_MAP, TRANSPARENT_MAP, TRANSPARENT_MAP, TRANSPARENT_MAP,	//First 4 are transparent
		0x597D27, 0x6D9930, 0x7FB238, 0x435E1D,
		0xAEA473, 0xD5C98C, 0xF7E9A3, 0x827B56,
		0x757575, 0x909090, 0xA7A7A7, 0x585858,
		0xB40000, 0xDC0000, 0xFF0000, 0x870000,
		0x7070B4, 0x8A8ADC, 0xA0A0FF, 0x545487,
		0x757575, 0x909090, 0xA7A7A7, 0x585858,
		0x005700, 0x006A00, 0x007C00, 0x004100,
		0xB4B4B4, 0xDCDCDC, 0xFFFFFF, 0x878787,
		0x737681, 0x8D909E, 0xA4A8B8, 0x565861,
		0x814A21, 0x9D5B28, 0xB76A2F, 0x603818,
		0x4F4F4F, 0x606060, 0x707070, 0x3B3B3B,
		0x2D2DB4, 0x3737DC, 0x4040FF, 0x212187,
		0x493A23, 0x59472B, 0x685332, 0x372B1A,
		0xB4B1AC, 0xDCD9D3, 0xFFFCF5, 0x878581,
		0x985924, 0xBA6D2C, 0xD87F33, 0x72431B,
		0x7D3598, 0x9941BA, 0xB24CD8, 0x5E2872,
		0x486C98, 0x5884BA, 0x6699D8, 0x365172,
		0xA1A124, 0xC5C52C, 0xE5E533, 0x79791B,
		0x599011, 0x6DB015, 0x7FCC19, 0x436C0D,
		0xAA5974, 0xD06D8E, 0xF27FA5, 0x804357,
		0x353535, 0x414141, 0x4C4C4C, 0x282828,
		0x6C6C6C, 0x848484, 0x999999, 0x515151,
		0x35596C, 0x416D84, 0x4C7F99, 0x284351,
		0x592C7D, 0x6D3699, 0x7F3FB2, 0x43215E,
		0x24357D, 0x2C4199, 0x334CB2, 0x1B285E,
		0x483524, 0x58412C, 0x664C33, 0x36281B,
		0x485924, 0x586D2C, 0x667F33, 0x36431B,
		0x6C2424, 0x842C2C, 0x993333, 0x511B1B,
		0x111111, 0x151515, 0x191919, 0x0D0D0D,
		0xB0A836, 0xD7CD42, 0xFAEE4D, 0x847E28,
		0x409A96, 0x4FBCB7, 0x5CDBD5, 0x307370,
		0x345AB4, 0x3F6EDC, 0x4A80FF, 0x274387,
		0x009928, 0x00BB32, 0x00D93A, 0x00721E,
		0x0E0E15, 0x12111A, 0x15141F, 0x0B0A10,
		0x4F0100, 0x600100, 0x700200, 0x3B0100			//The rest is padded
};

//This is the palette used by the Minecraft maps
//Retrieved from http://minecraft.gamepedia.com/Map_item_format#Map_colors
//Converted into arrays of {r,g,b}
#define DATPALETTE_SIZE 144
#define TRANSPARENT {255,0,255}	//Invalid pattern just cause
const unsigned char DATPalette[DATPALETTE_SIZE][3] = {
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
unsigned char TransparencyIndex = 0;

#define DATCOMPRESSION_SIZE 1
static const char* DATCompression[DATCOMPRESSION_SIZE] = {
		"NONE"
};

//Get the nearest color index for the given color
//Uses square Eucledian distance
//We're using this function rather than the one given by IM because that one sucks for some reason
#define square(x) (x)*(x)
extern "C" unsigned char nearest_color_index(unsigned char r, unsigned char g, unsigned char b)
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

extern "C" imImage* mapify(imImage* orig)
{
	//If the original file is already a DAT file, just dupe the given image and return it
	if(imImageGetAttribute(orig, "DAT", NULL, NULL))
		return imImageDuplicate(orig);

	//Get the original image's width and height, anything else will be modified
	imImage* mod = imImageCreateBased(orig, -1, -1, IM_MAP | IM_TOPDOWN, IM_BYTE);

	//Set the palette
	mod->palette = DATPaletteMap;

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
		unsigned char nearest_color = nearest_color_index(r, g, b);
		nova[i] = nearest_color;
		if(!nearest_color)
			nova[ppp + i] = 255;
	}

	return mod;
}

class DATFileFormat : public imFileFormatBase
{
	nbt_node *node, *data;
	FILE* fp = NULL;
	unsigned char* nbt = NULL;

public:
	DATFileFormat(const imFormat* _iformat): imFileFormatBase(_iformat) {}
	~DATFileFormat() {}

	int Open(const char* file_name);
	int New(const char* file_name);
	void Close();
	void* Handle(int index);
	int ReadImageInfo(int index);
	int ReadImageData(void* data);
	int WriteImageInfo();
	int WriteImageData(void* data);
};

class DATFormat : public imFormat
{
public:
	DATFormat() : imFormat("DAT", "Minecraft NBT Map", "*.dat", DATCompression, DATCOMPRESSION_SIZE, 0) {}
	~DATFormat() {}
	imFileFormatBase* Create(void) const { return new DATFileFormat(this); }
	int CanWrite(const char* compression, int color_mode, int data_type) const;
};

extern "C" void formatRegisterDAT()
{
	imFormatRegister(new DATFormat());
}

int DATFileFormat::Open(const char* file_name)
{
	//Open the DAT file and check that it is, in fact, a map
	this->node = nbt_parse_path(file_name);
	if (!this->node)												//Couldn't read the file, probably because it's not a valid NBT file
		return IM_ERR_OPEN;
	else if(!(this->data = nbt_find_by_name(this->node,"data")))	//Not a valid map file (i.e. doesn't have a "data" tag)
		return IM_ERR_DATA;
	//We should do more checks to make sure this is a valid map file, but in all honesty, why would anyone feed a fake or horribly mangled map file?

	//Set a few things required by IM
	strcpy(this->compression, "NONE");	//No compression(?)
	this->image_count = 1;				//One image only

	return IM_ERR_NONE;
}

int DATFileFormat::New(const char* file_name)
{
	//Open the file that we're going to write into
	if(!(this->fp = fopen(file_name, "wb")))
		return IM_ERR_OPEN;

	//Get a template NBT file
	if(!(this->node = create_nbt_template()) || !(this->data = nbt_find_by_name(this->node, "data")))
		return IM_ERR_MEM;

	this->image_count = 1;

	return IM_ERR_NONE;
}

void DATFileFormat::Close()
{
	if(this->node)
		nbt_free(this->node);
	if(this->fp)
		fclose(this->fp);
}

void* DATFileFormat::Handle(int index)
{
	return (void*)this->node;
}

int DATFileFormat::ReadImageInfo(int index)
{
	//Set the image type
	this->user_color_mode = this->file_color_mode = IM_MAP | IM_TOPDOWN;
	this->user_data_type = this->file_data_type = IM_BYTE;

	//Get width and height (and hope none of them are null)
	this->height = static_cast<int>(nbt_find_by_name(this->data, "height")->payload.tag_short);
	this->width = static_cast<int>(nbt_find_by_name(this->data, "height")->payload.tag_short);

	//Set the palette
	memcpy(this->palette, DATPaletteMap, 256*sizeof(long));

	//Tell everyone that this image is a DAT file and set the transparency index
	imAttribTable* attrib = AttribTable();
	attrib->SetInteger("DAT", IM_BYTE, 1);
	attrib->Set("TransparencyIndex", IM_BYTE, 1, &TransparencyIndex);

	return IM_ERR_NONE;
}

int DATFileFormat::ReadImageData(void* data)
{
	imCounterTotal(this->counter, this->height, "Reading DAT...");

	//Check that the colors exist and the size of the array makes sense
	nbt_node* colors = nbt_find_by_name(this->data, "colors");
	if(!colors)
		return IM_ERR_DATA;

	int size = colors->payload.tag_byte_array.length;
	if(this->width * this->height != size)
		return IM_ERR_DATA;

	//Fill the data with the colors
	for(int row = 0; row < this->height; row++)
	{
		memcpy(this->line_buffer, colors->payload.tag_byte_array.data + this->width*row, this->width);

		//Honestly, I don't know what this does, but the other files have this, so it must be important
		imFileLineBufferRead(this, data, row, 0);
		if(!imCounterInc(this->counter))
			return IM_ERR_COUNTER;
	}

	return IM_ERR_NONE;
}

int DATFileFormat::WriteImageInfo()
{
	this->user_color_mode = this->file_color_mode = IM_MAP;
	this->user_data_type = this->file_data_type = IM_BYTE;

	return IM_ERR_NONE;
}

int DATFileFormat::WriteImageData(void* data)
{
	imCounterTotal(this->counter, this->height, "Writing DAT...");

	nbt_node* colors = nbt_find_by_name(this->data, "colors");
	if(!colors)
		return IM_ERR_DATA;

	for(int row = 0; row < this->height; row++)
	{
		imFileLineBufferWrite(this, data, row, 0);
		memcpy(colors->payload.tag_byte_array.data + this->width*(this->height-row-1), this->line_buffer, this->width);	//gotta write it upside-down
		if (!imCounterInc(this->counter))
		      return IM_ERR_COUNTER;
	}

	if(nbt_dump_file(this->node, this->fp, STRAT_GZIP) != NBT_OK)
		return IM_ERR_ACCESS;

	return IM_ERR_NONE;
}

int DATFormat::CanWrite(const char* compression, int color_mode, int data_type) const {return 0;}
