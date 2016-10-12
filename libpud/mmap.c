/*
 * mmap.c
 * libpud
 *
 * Copyright (c) 2014 - 2016 Jean Guyomarc'h
 */

#include "pud_private.h"

#ifdef HAVE_MMAP
# include <fcntl.h>
# include <sys/stat.h>
# include <sys/mman.h>
# include <unistd.h>
#endif

void *
pud_mmap(const char *file,
         size_t     *size_ret)
{
#ifdef HAVE_MMAP

   void *map = NULL;
   struct stat s;
   int fd, chk;

   /* Open */
   fd = open(file, O_RDONLY, 0);
   if (fd == -1) DIE_RETURN(NULL, "Failed to open [%s]", file);

   /* Mmap */
   chk = fstat(fd, &s);
   if (chk < 0) DIE_GOTO(err_close, "Failed to fstat() [%s]", file);
   map = mmap(NULL, s.st_size, PROT_READ, MAP_FILE | MAP_PRIVATE, fd, 0);
   if (map == MAP_FAILED)
     {
        map = NULL;
        DIE_GOTO(err_close, "Failed to mmap() %s", strerror(errno));
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

void
pud_munmap(void   *map,
           size_t  size)
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
