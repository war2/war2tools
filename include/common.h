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

#include <stdio.h>
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
        fprintf(stderr,  "Failed to fstat(\"%s\")\n", file);
        goto err_close;
     }
   map = mmap(NULL, s.st_size, PROT_READ, MAP_FILE | MAP_PRIVATE, fd, 0);
   if (map == MAP_FAILED)
     {
        map = NULL;
        fprintf(stderr, "Failed to mmap(): %s\n", strerror(errno));
        goto err_close;
     }
   if (size_ret) *size_ret = s.st_size;

err_close:
   close(fd);
   return map;

#else
   fprintf(stderr,
           "*** %s() is not implemented on your platform!\n"
           "*** Please contact dev if you want support or add it by yourself.\n",
           __func__);
   return NULL;
#endif

}

static inline void
common_file_munmap(void *map, size_t size)
{
#ifdef HAVE_MMAP
   munmap(map, size);
#else
   fprintf(stderr,
           "*** %s() is not implemented on your platform!\n"
           "*** Please contact dev if you want support or add it by yourself.\n",
           __func__);
#endif
}

#endif /* ! __COMMON_H__ */
