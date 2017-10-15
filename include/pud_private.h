
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

#ifndef _PRIVATE_H_
#define _PRIVATE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <setjmp.h>
#include <time.h>

#include "debug.h"
#include "pud.h"

#include "common.h"

struct _Pud_Private
{
   Pud_Open_Mode  open_mode;
   Pud_Mmap *mem_map;

   /* Bitfield: is section X present? */
   uint32_t     sections;

   /* Cache */
   uint8_t       current_section;

   Pud_Bool has_erax;

   unsigned int  verbose;
   Pud_Bool init; /* set by defaults */
   Pud_Bool default_allow; /* [defaults] */
   Pud_Bool default_udta; /* [defaults] */
   Pud_Bool default_ugrd; /* [defaults] */
};

/* Visual hint when returning nothing */
#ifndef VOID
#define VOID
#endif

#define PUD_VERBOSE(pud, lvl, msg, ...) \
   do { \
      if (pud->private_data->verbose >= lvl) { \
         fprintf(stdout, "-- " msg "\n", ## __VA_ARGS__); \
      } \
   } while (0)

#define PUD_SANITY_CHECK(pud, mode, ...) \
   do { \
      if (!(pud)) { \
         DIE_RETURN(__VA_ARGS__, "Invalid PUD input [%p]", pud); \
      } else if (((pud)->private_data->open_mode & (mode)) != (mode)) { \
         DIE_RETURN(__VA_ARGS__, "PUD open mode is [%02x] expected [%02x]", \
                    (pud)->private_data->open_mode, mode); \
      } \
   } while (0)

#define PUD_CHECK_FERROR(f, ret) \
   do { \
      if (ferror(f)) { \
         fclose(f); \
         DIE_RETURN(ret, "Error while accessing file: %s", strerror(errno)); \
      } \
   } while (0)

#define PUD_TRAP_SETUP(pud_) COMMON_TRAP_SETUP((pud_)->private_data->mem_map)
#define PUD_READ8(pud_) common_read8((pud_)->private_data->mem_map)
#define PUD_READ16(pud_) common_read16((pud_)->private_data->mem_map)
#define PUD_READ32(pud_) common_read32((pud_)->private_data->mem_map)
#define PUD_READ_BUFFER(pud_, buf_, size_) common_read_buffer((pud_)->private_data->mem_map, buf_, size_)


//============================================================================//
//                                 Private API                                //
//============================================================================//

PUDAPI_INTERNAL const char *long2bin(uint32_t x);
PUDAPI_INTERNAL Pud_Color color_make(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
PUDAPI_INTERNAL const char *mode2str(Pud_Open_Mode mode);
PUDAPI_INTERNAL uint32_t pud_go_to_section(Pud *pud, Pud_Section sec);


#endif /* ! _PRIVATE_H_ */
