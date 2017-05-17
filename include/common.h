/*
 * Copyright (c) 2016 Jean Guyomarc'h
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

#ifdef HAVE_MMAP
# include <fcntl.h>
# include <sys/stat.h>
# include <sys/mman.h>
# include <unistd.h>
#endif

#ifdef HAVE_ACCESS
# include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static inline void *
common_file_mmap(const char *file, size_t *size_ret)
{
#ifdef HAVE_MMAP

   void *map = NULL;
   struct stat s;
   int fd, chk;

   /* Open */
   fd = open(file, O_RDONLY, 0);
   if (fd == -1)
     {
        fprintf(stderr, "*** Failed to open \"%s\"\n", file);
        return NULL;
     }

   /* Mmap */
   chk = fstat(fd, &s);
   if (chk < 0)
     {
        fprintf(stderr, "*** Failed to fstat(\"%s\")\n", file);
        goto err_close;
     }
   map = mmap(NULL, s.st_size, PROT_READ, MAP_FILE | MAP_PRIVATE, fd, 0);
   if (map == MAP_FAILED)
     {
        map = NULL;
        fprintf(stderr, "*** Failed to mmap(): %s\n", strerror(errno));
        goto err_close;
     }
   if (size_ret) *size_ret = s.st_size;

err_close:
   close(fd);
   return map;

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

   unsigned char *map = NULL, *tmp;
   char buf[4096];
   size_t size, total = 0;
   FILE *f;

   f = fopen(file, "rb");
   if (!f)
     {
        fprintf(stderr, "*** Failed to topen \"%s\"\n", file);
        return NULL;
     }

   do
     {
        size = fread(buf, sizeof(char), sizeof(buf), f);
        if (!size) break;

        total += size;
        tmp = realloc(map, total);
        if (!tmp)
          {
             fprintf(stderr, "*** realloc() failed\n");
             free(map); map = NULL; /* set to NULL to return */
             goto close;
          }
        map = tmp;
        memcpy(map + total - size, buf, size);
     }
   while ((!feof(f)) && (!ferror(f)));

   if (ferror(f))
     {
        fprintf(stderr, "*** ferror() was raised\n");
        free(map); map = NULL;
        goto close;
     }
   if (size_ret) *size_ret = total;

close:
   fclose(f);
   return map;
#endif
}

static inline void
common_file_munmap(void *map, size_t size)
{
#ifdef HAVE_MMAP
   munmap(map, size);
#else
   (void) size;
   free(map);
#endif
}

static inline int
common_file_exists(const char *path)
{
   if (!path) return 0;

#ifdef HAVE_ACCESS
   return (access(path, F_OK) == 0) ? 1 : 0;
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
   ok = (f == NULL) ? 0 : 1;
   fclose(f);
   return ok;

#endif
}

#endif /* ! __COMMON_H__ */
