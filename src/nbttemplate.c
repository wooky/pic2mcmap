/*
 * "WHAT THE HELL IS THIS FILE," I hear you scream.
 * Well, as far as I'm concerned, there's no easy way to create blank NBT files using cNBT.
 * Perhaps there is, but the only way I see is by using lists and buffers and all that, which is 3hardcore5me.
 * Instead, I'm going to use a hackish way, by creating a template NBT file and filling in the blanks later.
 * You see, the good thing about these NBT maps is that they're pretty much the same; the only thing that changes is the contents of the file.
 * Of course, this might not be true in the future when (and if) we'd have maps greater than 128x128, but for now, I'm going to assume that we're not in the future.
 * Here it goes!
 */

#include "header/nbttemplate.h"

#define NBT_SIZE_COMPOUND(x) 3+(x)
#define NBT_SIZE_BYTE(x) 4+(x)
#define NBT_SIZE_SHORT(x) 5+(x)
#define NBT_SIZE_INT(x) 7+(x)

#define NBT_DATA_OFFSET NBT_SIZE_COMPOUND(0)+NBT_SIZE_COMPOUND(4)+NBT_SIZE_BYTE(5)+NBT_SIZE_BYTE(9)+NBT_SIZE_SHORT(6)+NBT_SIZE_SHORT(5)+NBT_SIZE_INT(7)+NBT_SIZE_INT(7)+NBT_SIZE_INT(6)
#define NBT_TOTAL_SIZE NBT_DATA_OFFSET+16384+2

static unsigned char nbt_template[NBT_TOTAL_SIZE] = {
	TAG_COMPOUND, 0, 0,																//root tag, unnamed
		TAG_COMPOUND, 0, 4, 'd','a','t','a',											//map data tag
			TAG_BYTE,		0,5,	's','c','a','l','e',					4,				//map scale - set it to maximum (4) because we don't want to allow combining maps
			TAG_BYTE,		0,9,	'd','i','m','e','n','s','i','o','n',	1,				//put it in The End for the lulz (and to avoid accidentally overwriting the data)
			TAG_SHORT,		0,6,	'h','e','i','g','h','t',				0,128,			//map height of 128 (only possible value so far)
			TAG_SHORT,		0,5,	'w','i','d','t','h',					0,128,			//map width of 128 (only possible value so far)
			TAG_INT,		0,7,	'x','C','e','n','t','e','r',			127,255,255,0,	//some really big (positive) number
			TAG_INT,		0,7,	'z','C','e','n','t','e','r',			127,255,255,0,	//some really big (positive) number
			TAG_BYTE_ARRAY,	0,6,	'c','o','l','o','r','s',				0,0,64,0,		//16384 entries (=64*256)
		[NBT_TOTAL_SIZE - 2] = TAG_INVALID,												//END map data tag
	[NBT_TOTAL_SIZE - 1] = TAG_INVALID												//END root tag
};

nbt_node* nbt_template_create()
{
	return nbt_parse(nbt_template, NBT_TOTAL_SIZE);
}
