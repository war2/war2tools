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

#include "war2_private.h"
#include "common.h"

/*
 * I didn't find anywhere the specifications of .WAR files.
 * I used this: https://github.com/realhidden/wargus/blob/8677da9cd1c900056d37fdd2c933d1db1a93c395/wartool.cpp
 * and hexdump.
 */

PUDAPI Pud_Bool
war2_init(void)
{
   return PUD_TRUE;
}

PUDAPI void
war2_shutdown(void)
{
   /* Nothing to do */
}


static Pud_Bool
_palette_extract(War2_Data *w2, unsigned int entry,
                 Pud_Color *palette)
{
   unsigned char *ptr;
   size_t size;
   unsigned int i;

   ptr = war2_entry_extract(w2, entry, &size);
   if (!ptr)
     DIE_RETURN(PUD_FALSE, "Failed to extract entry palette [%u]", entry);
   if (size != 768)
     {
        free(ptr);
        DIE_RETURN(PUD_FALSE, "Invalid size [%zu]. Should be 256*3=768", size);
     }

   /* I don't know why is the bitshift needed (no doc so no explaination) but
    * this gives the right colorspace (thanks wargus) */
   for (i = 0; i < WAR2_PALETTE_SIZE; ++i)
     {
        const unsigned char *const p = &(ptr[i * 3]);
        palette[i].r = p[0] << 2;
        palette[i].g = p[1] << 2;
        palette[i].b = p[2] << 2;
        palette[i].a = 0xff;
     }
   palette[0].a = 0x00;

   free(ptr);
   return PUD_TRUE;
}

PUDAPI War2_Data *
war2_open(const char *file)
{
   War2_Data *w2;
   int i;
   uint32_t l;

   /* Safety check the input */
   if (!file) DIE_GOTO(err, "NULL input file");

   /* Allocate memory and set verbosity */
   w2 = calloc(1, sizeof(War2_Data));
   if (!w2) DIE_GOTO(err, "Failed to allocate memory");

   /* Map file */
   w2->mem_map = common_file_mmap(file);
   if (!w2->mem_map) DIE_GOTO(err_free, "Failed to map file");

   WAR2_TRAP_SETUP(w2) {
      if (w2->entries) free(w2->entries);
err_unmap:
      common_file_munmap(w2->mem_map);
err_free:
      free(w2);
      return NULL;
   }

   /* Read magic */
   w2->magic = WAR2_READ32(w2);
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
   w2->entries_count = WAR2_READ16(w2);

   /* File ID */
   w2->fid = WAR2_READ16(w2);

   /* Allocate entries table */
   w2->entries = calloc(w2->entries_count, sizeof(unsigned char *));
   if (!w2->entries) DIE_GOTO(err_unmap, "Failed to allocate memory");

   /* Register all entries */
   for (i = 0; i < w2->entries_count; i++)
     {
        /* Get offset and set starting point of sections */
        l = WAR2_READ32(w2);
        if (l >= w2->mem_map->size)
          {
             ERR("Entry %i has offset [%u] larger than file size [%zu]. Skipping...",
                 i, l, w2->mem_map->size);
             w2->entries[i] = NULL;
             continue;
          }
        w2->entries[i] = (unsigned char*)(w2->mem_map->map) + l;
     }

   _palette_extract(w2, 2, w2->forest);
   _palette_extract(w2, 18, w2->winter);
   _palette_extract(w2, 10, w2->wasteland);
   _palette_extract(w2, 438, w2->swamp);

   return w2;

err:
   return NULL;
}

PUDAPI const Pud_Color *
war2_palette_get(const War2_Data *w2, Pud_Era era)
{
   switch (era)
     {
      case PUD_ERA_FOREST: return w2->forest;
      case PUD_ERA_WINTER: return w2->winter;
      case PUD_ERA_WASTELAND: return w2->wasteland;
      case PUD_ERA_SWAMP: return w2->swamp;
      default: return NULL;
     }
}


PUDAPI unsigned char *
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

   WAR2_TRAP_SETUP(w2) {
fail:
      if (size_ret) *size_ret = 0;
      free(ptr);
      return NULL;
   }

   /* Go at entry */
   w2->mem_map->ptr = w2->entries[entry];

   /* Uncompressed length (3 bytes) & Flags (1 byte) */
   l = WAR2_READ32(w2);
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
         memcpy(ptr, w2->mem_map->ptr, ulen);
         break;

      case 0x20: // Compressed
         memset(&(buf[0]), 0, sizeof(buf));
         p = ptr;
         e = ptr + ulen;
         while (p < e)
           {
              bits = WAR2_READ8(w2);
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
                        b = WAR2_READ8(w2);
                        *(p++) = b;
                        buf[bi++ & 0xfff] = b;
                     }
                   else
                     {
                        w = WAR2_READ16(w2);
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
}

PUDAPI void
war2_close(War2_Data *w2)
{
   if (!w2) return;
   common_file_munmap(w2->mem_map);
   free(w2->entries);
   free(w2);
}

PUDAPI void
war2_verbosity_set(War2_Data *w2,
                   int        level)
{
   if (w2) w2->verbose = level;
}
