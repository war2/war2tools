#ifndef _PRIVATE_C_
#define _PRIVATE_C_

#include "war2_private.h"

Pud_Bool
war2_mem_map_ok(War2_Data *w2)
{
   return (w2->ptr < w2->mem_map + w2->mem_map_size);
}

#endif /* ! _PRIVATE_C_ */

