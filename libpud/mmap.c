/*
 * mmap.c
 * libpud
 *
 * Copyright (c) 2014 Jean Guyomarc'h
 */

#include "pud_private.h"

#if defined(__linux__) || (defined(__APPLE__) && defined(__MACH__))
# define POSIX_COMPLIANT 1
#else
# define POSIX_COMPLIANT 0
#endif

#if POSIX_COMPLIANT
# include <fcntl.h>
# include <sys/stat.h>
# include <sys/mman.h>
# include <unistd.h>
#endif

#if POSIX_COMPLIANT /* ... */
# define PUD_MMAP_ENABLED 1
#else
# define PUD_MMAP_ENABLED 0
#endif

void *
pud_mmap(const char *file,
         size_t     *size_ret)
{
#if PUD_MMAP_ENABLED

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
#if PUD_MMAP_ENABLED
   munmap(map, size);
#else
   fprintf(stderr,
           "*** %s() is not implemented on your platform!\n"
           "*** Please contact dev if you want support or add it by yourself.\n",
           __func__);
#endif
}


