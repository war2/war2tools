#include "war2_private.h"

/*
 * I didn't find anywhere the specifications of .WAR files.
 * I used this: https://github.com/realhidden/wargus/blob/8677da9cd1c900056d37fdd2c933d1db1a93c395/wartool.cpp
 * and hexdump.
 */

bool
war2_init(void)
{
   return pud_init();
}

void
war2_shutdown(void)
{
   pud_shutdown();
}

War2_Data *
war2_open(const char *file,
          int         verbosity)
{
   War2_Data *w2;
   int i;
   uint32_t l;

   /* Allocate memory and set verbosity */
   w2 = calloc(1, sizeof(War2_Data));
   if (!w2) DIE_GOTO(err, "Failed to allocate memory");
   war2_verbosity_set(w2, verbosity);

   /* Map file */
   w2->mem_map = pud_mmap(file, &(w2->mem_map_size));
   if (!w2->mem_map) DIE_GOTO(err_free, "Failed to map file");
   w2->ptr = w2->mem_map;
   WAR2_VERBOSE(w2, 1, "File [%s] mapped size is %zu bytes", file, w2->mem_map_size);

   /* Read magic */
   w2->magic = READ32(w2, ECHAP(err_unmap));
   switch (w2->magic)
     {
      case 0x00000019: // Handled
         WAR2_VERBOSE(w2, 1, "File [%s] has magic [0x%08x]", file, w2->magic);
         break;

      default:
         ERR("Unknown or (yet) unsupported magic [0x%08x]", w2->magic);
         goto err_unmap;
     }

   /* Get the entries */
   w2->entries_count = READ16(w2, ECHAP(err_unmap));
   WAR2_VERBOSE(w2, 1, "File [%s] has [%u] entries", file, w2->entries_count);

   /* File ID */
   w2->fid = READ16(w2, ECHAP(err_unmap));
   WAR2_VERBOSE(w2, 1, "File [%s] has ID [0x%04x]", file, w2->fid);

   /* Allocate entries table */
   w2->entries = calloc(w2->entries_count, sizeof(unsigned char *));
   if (!w2->entries) DIE_GOTO(err_unmap, "Failed to allocate memory");

   /* Register all entries */
   for (i = 0; i < w2->entries_count; i++)
     {
        /* Get offset and set starting point of sections */
        l = READ32(w2, ECHAP(err_free_all));
        if (l >= w2->mem_map_size)
          {
             ERR("Entry %i has offset [%u] larger than file size [%zu]. Skipping...",
                 i, l, w2->mem_map_size);
             w2->entries[i] = NULL;
             continue;
          }
        w2->entries[i] = w2->mem_map + l;
        WAR2_VERBOSE(w2, 3, "Entry %i has offset of %u", i, l);
     }

   return w2;

err_free_all:
   free(w2->entries);
err_unmap:
   pud_munmap(w2->mem_map, w2->mem_map_size);
err_free:
   free(w2);
err:
   return NULL;
}

unsigned char *
war2_entry_extract(War2_Data    *w2,
                   unsigned int  entry,
                   size_t       *size_ret)
{
   unsigned char buf[4096];
   unsigned char *ptr = NULL, *p, *e;
   uint32_t l, ulen;
   uint16_t w;
   uint8_t bits, b;
   int flags, i, j, bi = 0;

   /* Check the entry is in the range */
   if (entry >= w2->entries_count)
     DIE_RETURN(NULL, "Invalid entry [%i]. Entries range is: [0 ; %u].",
                entry, w2->entries_count - 1);

   /* Go at entry */
   w2->ptr = w2->entries[entry];

   /* Uncompressed length (3 bytes) & Flags (1 byte) */
   l = READ32(w2, FAIL(NULL));
   flags = l >> 24;
   ulen = l & 0x00ffffff;
   WAR2_VERBOSE(w2, 2, "Entry %i: uncompressed length: %i. Flags: 0x%02x",
                entry, ulen, flags);

   /* Output entry will always be duplicated */
   ptr = malloc(ulen);
   if (!ptr) DIE_RETURN(NULL," Failed to allocate memory");

   switch (flags)
     {
      case 0x00: // Uncompressed
         memcpy(ptr, w2->ptr, ulen);
         break;

      case 0x20: // Compressed
         memset(&(buf[0]), 0, sizeof(buf));
         p = ptr;
         e = ptr + ulen;
         while (p < e)
           {
              bits = READ8(w2, ECHAP(fail));
              for (i = 0; i < 8; i++)
                {
                   /*
                    * XXX
                    * I don't know what's the (de)compression method used here.
                    * I blindly rely on the implementation of Wargus until I exactly
                    * figure out what's going on (I'm a bit in the hurry right now).
                    */
                   if (bits & 1)
                     {
                        b = READ8(w2, ECHAP(fail));
                        *(p++) = b;
                        buf[bi++ & 0xfff] = b;
                     }
                   else
                     {
                        w = READ16(w2, ECHAP(fail));
                        j = (w >> 12) + 3;
                        w &= 0x0fff;
                        while (j--)
                          {
                             buf[bi++ & 0xfff] = *(p++) = buf[w++ & 0xfff];
                             if (p == e) break;
                          }
                     }
                   if (p == e) break;
                   bits >>= 1;
                }
           }
         break;

      default:
         DIE_GOTO(fail, "Unhandled flags [0x%02x] for entry %i", flags, entry);
     }

   WAR2_VERBOSE(w2, 1, "Extracted entry [%i] of size %i bytes", entry, ulen);
   if (size_ret) *size_ret = ulen;
   return ptr;

fail:
   if (size_ret) *size_ret = 0;
   free(ptr);
   return NULL;
}

void
war2_close(War2_Data *w2)
{
   if (!w2) return;
   pud_munmap(w2->mem_map, w2->mem_map_size);
   free(w2->entries);
   free(w2);
}

void
war2_verbosity_set(War2_Data *w2,
                   int        level)
{
   if (w2) w2->verbose = level;
}


