/*
 * Copyright (c) 2016-2017 Jean Guyomarc'h
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

#ifndef __COMMON_H__
#define __COMMON_H__

#include "debug.h"
#include <stddef.h>
#include <setjmp.h>

#ifdef __GNUC__
# if __GNUC__ >= 4
#  define PUDAPI_INTERNAL __attribute__ ((visibility("hidden")))
# else /* if __GNUC__ >= 4 */
#  define PUDAPI_INTERNAL
# endif /* if __GNUC__ >= 4 */
#else /* ifdef __GNUC__ */
# define PUDAPI_INTERNAL
#endif /* ifdef __GNUC__ */

typedef struct _Pud_Mmap Pud_Mmap;

struct _Pud_Mmap
{
   void *map;
   unsigned char *ptr;
   size_t size;
   jmp_buf trap;
};

PUDAPI Pud_Mmap *common_file_mmap(const char *file);
PUDAPI void common_file_munmap(Pud_Mmap *map);
PUDAPI Pud_Bool common_file_exists(const char *path);
PUDAPI Pud_Bool common_mem_map_ok(const Pud_Mmap *map, size_t extra);
PUDAPI void common_mmap_ptr_reset(Pud_Mmap *map);
PUDAPI void common_read_buffer(Pud_Mmap *map, void *buf, size_t bytes);
PUDAPI uint8_t common_read8(Pud_Mmap *map);
PUDAPI uint16_t common_read16(Pud_Mmap *map);
PUDAPI uint32_t common_read32(Pud_Mmap *map);
PUDAPI void common_trap_hook(void);

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define COMMON_TRAP_SETUP(Map) \
   if (setjmp((Map)->trap) != 0)

#define COMMON_TRAP_RAISE(Map, ...)      \
  do {                                   \
     ERR(__VA_ARGS__);                   \
     common_trap_hook();                 \
     longjmp((Map)->trap, 1);            \
  } while (0)

#endif /* ! __COMMON_H__ */
