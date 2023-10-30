/*
 * Copyright (c) 2017 Jean Guyomarc'h
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


#ifdef HAVE_MMAP
# include <fcntl.h>
# include <sys/stat.h>
# include <sys/mman.h>
# include <unistd.h>
#endif

#if defined(HAVE_MSVC)
# include <io.h>
#elif defined(HAVE_ACCESS)
# include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "pud.h"
#include "common.h"


PUDAPI Pud_Mmap *
common_file_mmap(const char *file)
{
   Pud_Mmap *const map = calloc(1, sizeof(Pud_Mmap));
   if (! map) return NULL;

#ifdef HAVE_MMAP

   struct stat s;
   int fd, chk;

   /* Open */
   fd = open(file, O_RDONLY, 0);
   if (fd == -1)
     {
        fprintf(stderr, "*** Failed to open \"%s\"\n", file);
        goto free_map;
     }

   /* Mmap */
   chk = fstat(fd, &s);
   if (chk < 0)
     {
        fprintf(stderr, "*** Failed to fstat(\"%s\")\n", file);
        goto err_close;
     }
   map->map = mmap(NULL, s.st_size, PROT_READ, MAP_FILE | MAP_PRIVATE, fd, 0);
   if (map->map == MAP_FAILED)
     {
        fprintf(stderr, "*** Failed to mmap(): %s\n", strerror(errno));
        goto err_close;
     }
   map->size = s.st_size;
   map->ptr = map->map;

   close(fd);
   return map;
err_close:
   close(fd);
free_map:
   free(map);
   return NULL;

#else

   /*
    * Fallback... mmap() will be damn better, but that for lame
    * systems...
    * I have a buffer, read the file in the buffer, and every time
    * I copy the buffer (from the stack) to a persistant buffer
    * in the heap, which is dynamically resized for every buffer
    * read. Yes, that's lame, but that's platform agnostic...
    * Cannot pre-alloc the heap buffer because I have no simple way
    * to detect the size of the file. SEEK_SET (for fseek()) is not
    * portable enough to be used... so no... no ftell() :/
    * I believe this way is much faster than going through the file
    * twice.
    */

   unsigned char *mem = NULL, *tmp;
   char buf[4096];
   size_t size, total = 0;
   FILE *f;

   f = fopen(file, "rb");
   if (!f)
     {
        fprintf(stderr, "*** Failed to topen \"%s\"\n", file);
        goto free_map;
     }

   do
     {
        size = fread(buf, sizeof(char), sizeof(buf), f);
        if (!size) break;

        total += size;
        tmp = realloc(mem, total);
        if (!tmp)
          {
             fprintf(stderr, "*** realloc() failed\n");
             free(mem);
             goto close;
          }
        mem = tmp;
        memcpy(mem + total - size, buf, size);
     }
   while ((!feof(f)) && (!ferror(f)));

   if (ferror(f))
     {
        fprintf(stderr, "*** ferror() was raised\n");
        free(mem);
        goto close;
     }
   map->map = mem;
   map->size = total;
   map->ptr = map->map;

   fclose(f);
   return map;
close:
   fclose(f);
free_map:
   free(map);
   return NULL;
#endif
}

PUDAPI void
common_file_munmap(Pud_Mmap *map)
{
#ifdef HAVE_MMAP
   munmap(map->map, map->size);
#else
   free(map->map);
#endif
   free(map);
}

PUDAPI void
common_mmap_ptr_reset(Pud_Mmap *map)
{
   map->ptr = map->map;
}

PUDAPI Pud_Bool
common_file_exists(const char *path)
{
   if (!path) return PUD_FALSE;

#ifdef HAVE_MSVC
   return (_access_s(path, 0) == 0) ? PUD_TRUE : PUD_FALSE;
#elif defined(HAVE_ACCESS)
   return (access(path, F_OK) == 0) ? PUD_TRUE : PUD_FALSE;
#else

   /*
    * Fallback. Not very good, because it does not take
    * account of access rights. fopen could fail for other
    * reasons then a non-existant file. But that is just
    * a fallback, so it should do...
    */

   FILE *f;
   int ok;

   f = fopen(path, "r");
   ok = (f == NULL) ? PUD_FALSE : PUD_TRUE;
   fclose(f);
   return ok;

#endif
}

PUDAPI void
common_trap_hook(void)
{
   /* Do nothing. The debugger will hook there if need be. */
}

PUDAPI Pud_Bool
common_mem_map_ok(const Pud_Mmap *map, size_t extra)
{
   return (map->ptr + extra <= (unsigned char *)map->map + map->size)
      ? PUD_TRUE : PUD_FALSE;
}

PUDAPI void
common_read_buffer(Pud_Mmap *map, void *buf, size_t bytes)
{
  if (! common_mem_map_ok(map, bytes))
    COMMON_TRAP_RAISE(map, "Trying to read outside of the memory map");
  memcpy(buf, map->ptr, bytes);
  map->ptr += bytes;
}

#define READ_FUNCTION_TEMPLATE(Name, Type)                              \
  PUDAPI Type Name(Pud_Mmap *map)                                       \
  {                                                                     \
    Type storage;                                                       \
    if (! common_mem_map_ok(map, sizeof(Type))) {                       \
      COMMON_TRAP_RAISE(map, "Trying to read outside of the memory map");  \
    }                                                                   \
    memcpy(&storage, map->ptr, sizeof(Type));                           \
    map->ptr += sizeof(Type);                                           \
    return storage;                                                     \
  }

READ_FUNCTION_TEMPLATE(common_read8, uint8_t)
READ_FUNCTION_TEMPLATE(common_read16, uint16_t)
READ_FUNCTION_TEMPLATE(common_read32, uint32_t)
