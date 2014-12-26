//Oh joy it's a C++ file in a C project. Nothing could possibly go wrong here
#include "header/datformat.hpp"

#include "header/nbt/nbt.h"

#include <im_format.h>

#define DATCOMPRESSION_SIZE 2
static const char* DATCompression[DATCOMPRESSION_SIZE] = {
		"NONE",	//Not used
		"GZIP"
};

class DATFileFormat : public imFileFormatBase
{
	nbt_node* node;

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
	node = nbt_parse_path(file_name);
	return IM_ERR_NONE;
}

int DATFileFormat::New(const char* file_name) {return 0;}

void DATFileFormat::Close()
{
	nbt_free(node);
}

void* DATFileFormat::Handle(int index) {return 0;}

int DATFileFormat::ReadImageInfo(int index)
{
	this->height = static_cast<int>(nbt_find_by_name(node, "height")->payload.tag_short);
	this->width = static_cast<int>(nbt_find_by_name(node, "height")->payload.tag_short);
	return IM_ERR_NONE;
}

int DATFileFormat::ReadImageData(void* data) {return 0;}

int DATFileFormat::WriteImageInfo() {return 0;}

int DATFileFormat::WriteImageData(void* data) {return 0;}

int DATFormat::CanWrite(const char* compression, int color_mode, int data_type) const {return 0;}
