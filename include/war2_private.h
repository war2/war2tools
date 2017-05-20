/*
 * Copyright (c) 2014-2016 Jean Guyomarc'h
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef _WAR2_PRIVATE_H_
#define _WAR2_PRIVATE_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>

#include "pud.h"
#include "pud_private.h"
#include "debug.h"
#include "war2.h"

#undef FAIL
#undef ECHAP
#undef READ8
#undef READ16
#undef READ32
#undef READBUF


struct _War2_Data
{
   unsigned char *mem_map;
   size_t         mem_map_size;
   unsigned char *ptr;

   uint32_t     magic;
   uint16_t     fid;

   uint16_t        entries_count;
   unsigned char **entries;

   Pud_Color forest[WAR2_PALETTE_SIZE];
   Pud_Color winter[WAR2_PALETTE_SIZE];
   Pud_Color wasteland[WAR2_PALETTE_SIZE];
   Pud_Color swamp[WAR2_PALETTE_SIZE];

   int verbose;
};


#define FAIL(ret_) return ret_
#define ECHAP(lab_) goto lab_

#define READ8(w, ...) \
   ({ \
    uint8_t x__[1]; \
    const size_t size__ = sizeof(x__[0]); \
    if (!(war2_mem_map_ok(w))) { \
    ERR("Read outside of memory map!"); \
    __VA_ARGS__; \
    } \
    memcpy(&(x__[0]), w->ptr, size__); \
    w->ptr += size__; \
    x__[0]; \
    })

#define READ16(w, ...) \
   ({ \
    uint16_t x__[1]; \
    const size_t size__ = sizeof(x__[0]); \
    if (!(war2_mem_map_ok(w))) { \
    ERR("Read outside of memory map!"); \
    __VA_ARGS__; \
    } \
    memcpy(&(x__[0]), w->ptr, size__); \
    w->ptr += size__; \
    x__[0]; \
    })

#define READ32(w, ...) \
   ({ \
    uint32_t x__[1]; \
    const size_t size__ = sizeof(x__[0]); \
    if (!(war2_mem_map_ok(w))) { \
    ERR("Read outside of memory map!"); \
    __VA_ARGS__; \
    } \
    memcpy(&(x__[0]), w->ptr, size__); \
    w->ptr += size__; \
    x__[0]; \
    })

#define READBUF(w, buf, type, count, ...) \
   do { \
      void *ptr__ = (buf); \
      const size_t size__ = sizeof(type) * (count); \
      if (!(war2_mem_map_ok(w))) { \
         ERR("Read outside of memory map!"); \
         __VA_ARGS__; \
      } \
      memcpy(ptr__, w->ptr, size__); \
      w->ptr += size__; \
   } while (0)


#define WAR2_VERBOSE(w2, lvl, msg, ...) \
   do { \
      if (w2->verbose >= lvl) { \
         fprintf(stdout, "-- " msg "\n", ## __VA_ARGS__); \
      } \
   } while (0)

static inline Pud_Bool
war2_mem_map_ok(War2_Data *w2)
{
   return (w2->ptr < w2->mem_map + w2->mem_map_size)
      ? PUD_TRUE : PUD_FALSE;
}

#endif /* ! _WAR2_PRIVATE_H_ */
