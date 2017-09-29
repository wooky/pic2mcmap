//Oh joy it's a C++ file in a C project. Nothing could possibly go wrong here
#include "datformat.hpp"

#include <im_format.h>	//Can't shove this into the .hpp file here

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

#define DATCOMPRESSION_SIZE 1
static const char* DATCompression[DATCOMPRESSION_SIZE] = {
		"NONE"
};

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
	if(!(this->node = nbt_template_create()) || !(this->data = nbt_find_by_name(this->node, "data")))
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
	//Init the attribute table
	imAttribTable* attrib = AttribTable();
	attrib->RemoveAll();
	imFileSetBaseAttributes(this);

	//Set the image type
	this->user_color_mode = this->file_color_mode = IM_MAP | IM_TOPDOWN;
	this->user_data_type = this->file_data_type = IM_BYTE;

	//Get width and height (and hope none of them are null)
	this->height = static_cast<int>(nbt_find_by_name(this->data, "height")->payload.tag_short);
	this->width = static_cast<int>(nbt_find_by_name(this->data, "height")->payload.tag_short);

	//Set the palette
	memcpy(this->palette, DATPaletteMap, 256*sizeof(long));

	//Tell everyone that this image is a DAT file and set the transparency index
	attrib->SetInteger("DAT", IM_BYTE, 1);
	attrib->SetInteger("TransparencyIndex", IM_BYTE, 0);

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

int DATFormat::CanWrite(const char* compression, int color_mode, int data_type) const
{
	//Whatever
	return IM_ERR_NONE;
}
