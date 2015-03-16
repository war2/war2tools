/*
 * private.c
 * libwar2
 *
 * Copyright (c) 2014 Jean Guyomarc'h
 */

#include "war2_private.h"

Pud_Bool
war2_mem_map_ok(War2_Data *w2)
{
   return (w2->ptr < w2->mem_map + w2->mem_map_size);
}

