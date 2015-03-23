/*
 * units.c
 * libwar2
 *
 * Copyright (c) 2015 Jean Guyomarc'h
 */

#include "war2_private.h"

#define RLE_REPEAT (1 << 6)
#define RLE_LEAVE  (1 << 7)
#define PALETTE_ALPHA 0

typedef struct {
   unsigned char r;
   unsigned char g;
   unsigned char b;
} Col;

static const Col _colors[8][4] =
{
   { /* Red */
        { 0x44, 0x04, 0x00 },
        { 0x5c, 0x04, 0x00 },
        { 0x7c, 0x00, 0x00 },
        { 0xa4, 0x00, 0x00 },
   },
   { /* Blue */
        { 0x00, 0x04, 0x4c },
        { 0x00, 0x14, 0x6c },
        { 0x00, 0x24, 0x94 },
        { 0x00, 0x3c, 0xc0 }
   },
   { /* Green */
        { 0x00, 0x3c, 0xc0 },
        { 0x04, 0x54, 0x2c },
        { 0x14, 0x84, 0x5c },
        { 0x2c, 0xb4, 0x94 }
   },
   { /* Violet */
        { 0x2c, 0x08, 0x2c },
        { 0x50, 0x10, 0x4c },
        { 0x74, 0x30, 0x84 },
        { 0x98, 0x48, 0xb0 }
   },
   { /* Orange */
        { 0x6e, 0x20, 0x0c },
        { 0x98, 0x38, 0x10 },
        { 0xc4, 0x58, 0x10 },
        { 0xf0, 0x84, 0x14 }
   },
   { /* Black */
        { 0x0c, 0x0c, 0x14 },
        { 0x14, 0x14, 0x20 },
        { 0x1c, 0x1c, 0x2c },
        { 0x28, 0x28, 0x3c }
   },
   { /* White */
        { 0x24, 0x28, 0x4c },
        { 0x54, 0x54, 0x80 },
        { 0x98, 0x98, 0xb4 },
        { 0xe0, 0xe0, 0xe0 }
   },
   { /* Yellow */
        { 0xb4, 0x74, 0x00 },
        { 0xcc, 0xa0, 0x10 },
        { 0xe4, 0xcc, 0x28 },
        { 0xfc, 0xfc, 0x48 }
   }
};


static void
_sprites_colorize(Pud_Color    *sprite,
                  unsigned int  size,
                  Pud_Player    color)
{
   unsigned int i, k;
   Pud_Color *col;

   if (color == PUD_PLAYER_RED) return;

   for (k = 0; k < size; ++k)
     {
        col = &(sprite[k]);
        for (i = 0; i < 4; ++i)
          {
             if (!memcmp(col, &(_colors[0][i]), sizeof(Col)))
               {
                  memcpy(col, &(_colors[color][i]), sizeof(Col));
                  break;
               }
          }
     }
}


static Pud_Bool
_sprites_entries_parse(War2_Data                *w2,
                       War2_Sprites_Descriptor  *ud,
                       const unsigned int       *entries,
                       War2_Sprites_Decode_Func  func)
{
   unsigned char *ptr;
   uint16_t count, i, oline, max_w, max_h;
   uint8_t x, y, w, h, c;
   uint32_t dstart;
   size_t size, max_size;
   unsigned int offset, l, pcount, k;
   unsigned char *img = NULL, *pimg, *rows, *o;
   Pud_Color *img_rgba = NULL;

   /* If no callback has been specified, do nothing */
   if (!func)
     {
        WAR2_VERBOSE(w2, 1, "Warning: No callback specified.");
        return PUD_TRUE;
     }

   /* Palette */
   ptr = war2_palette_extract(w2, entries[0]);
   if (!ptr) DIE_RETURN(PUD_FALSE, "Failed to get palette");
   war2_palette_convert(ptr, ud->palette);
   free(ptr);

   ptr = war2_entry_extract(w2, entries[1], &size);
   if (!ptr) DIE_RETURN(PUD_FALSE, "Failed to extract entry");

   memcpy(&count, &(ptr[0]), sizeof(uint16_t));
   memcpy(&max_w, &(ptr[2]), sizeof(uint16_t));
   memcpy(&max_h, &(ptr[4]), sizeof(uint16_t));

   max_size = max_w * max_h;
   img = malloc(max_size * sizeof(unsigned char));
   img_rgba = malloc(max_size * sizeof(Pud_Color));

   for (i = 0, offset = 6; i < count; ++i, offset += 8)
     {
        memcpy(&x, &(ptr[offset + 0]), sizeof(uint8_t));
        memcpy(&y, &(ptr[offset + 1]), sizeof(uint8_t));
        memcpy(&w, &(ptr[offset + 2]), sizeof(uint8_t));
        memcpy(&h, &(ptr[offset + 3]), sizeof(uint8_t));
        memcpy(&dstart, &(ptr[offset + 4]), sizeof(uint32_t));

        rows = ptr + dstart;
        pimg = img;

        for (l = 0; l < h; ++l)
          {
             memcpy(&oline, rows + (l * sizeof(uint16_t)), sizeof(uint16_t));
             o = rows + oline;

             for (pcount = 0; pcount < w;)
               {
                  c = *(o++);
                  if (c & RLE_REPEAT)
                    {
                       /* Repeat the next byte (c \ RLE_REPEAT) times as pixel value */
                       c &= 0x3f;
                       memset(&(pimg[pcount]), *(o++), c);
                       pcount += c;
                    }
                  else if (c & RLE_LEAVE)
                    {
                       /* Leave (c \ RLE_LEAVE) pixels transparent */
                       c &= 0x7f;
                       memset(&(pimg[pcount]), PALETTE_ALPHA, c);
                       pcount += c;
                    }
                  else
                    {
                       /* Take the next (c) bytes as pixel values */
                       memcpy(&(pimg[pcount]), o, c);
                       pcount += c;
                       o += c;
                    }
               }
             pimg += pcount;
          }

        size = w * h;
        for (k = 0; k < size; ++k)
          img_rgba[k] = ud->palette[img[k]];

        _sprites_colorize(img_rgba, size, ud->color);
        func(img_rgba, w, h, ud, i);
     }

   free(img_rgba);
   free(img);
   free(ptr);

   return PUD_TRUE;
}


War2_Sprites_Descriptor *
war2_sprites_decode(War2_Data                *w2,
                    Pud_Player                player_color,
                    Pud_Side                  race,
                    War2_Sprites_Decode_Func  func)
{
   War2_Sprites_Descriptor *ud;
   const unsigned int entries[] = { 2, 33 };

   /* Alloc */
   ud = calloc(1, sizeof(*ud));
   if (!ud) DIE_RETURN(NULL, "Failed to allocate memory");
   ud->race = race;
   ud->color = player_color;

   _sprites_entries_parse(w2, ud, entries, func);

   return ud;
}

