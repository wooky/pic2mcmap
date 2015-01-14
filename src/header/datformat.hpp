#ifndef HEADER_DATFORMAT_HPP_
#define HEADER_DATFORMAT_HPP_

#include "nbt/nbt.h"
#include "nbttemplate.h"

#include <im_file.h>
#include <im_image.h>
#include <im_counter.h>

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

extern long DATPaletteMap[256];
void formatRegisterDAT();

#ifdef __cplusplus
}
#endif

#endif /* HEADER_DATFORMAT_HPP_ */
