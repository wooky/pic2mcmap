#ifndef HEADER_NBTTEMPLATE_H_
#define HEADER_NBTTEMPLATE_H_

#include "nbt/nbt.h"

#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void nbt_template_setup();
nbt_node* nbt_template_create();

#ifdef __cplusplus
}
#endif

#endif /* HEADER_NBTTEMPLATE_H_ */
