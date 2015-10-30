/*
 * war2.h
 * libwar2
 *
 * Copyright (c) 2014 - 2015 Jean Guyomarc'h
 */

#ifndef _WAR2_H_
#define _WAR2_H_
#ifdef __cplusplus
extern "C" <%
#endif

#include "pud.h"

Pud_Bool war2_init(void);
void war2_shutdown(void);

typedef struct _War2_Data War2_Data;
typedef struct _War2_Tileset_Descriptor War2_Tileset_Descriptor;
typedef struct _War2_Sprites_Descriptor War2_Sprites_Descriptor;
typedef struct _War2_Color War2_Color;

typedef enum
{
   WAR2_SPRITES_UNITS     = 0x100,
   WAR2_SPRITES_BUILDINGS = 0x101,
   WAR2_SPRITES_ICONS     = 0x102,
   WAR2_SPRITES_SYSTEM    = 0x103
} War2_Sprites;

struct _War2_Data
{
   unsigned char *mem_map;
   size_t         mem_map_size;
   unsigned char *ptr;

   uint32_t     magic;
   uint16_t     fid;

   uint16_t        entries_count;
   unsigned char **entries;

   unsigned int verbose;
};

struct _War2_Tileset_Descriptor
{
   Pud_Era       era;
   Pud_Color     palette[256];
   int           tiles;
};

struct _War2_Sprites_Descriptor
{
   Pud_Color    palette[256];

   Pud_Player   color;
   Pud_Era      era;
   Pud_Side     side;
   Pud_Unit     object;
   War2_Sprites sprite_type;

   int          count;
   Pud_Side     race;
};

typedef void (*War2_Tileset_Decode_Func)(const Pud_Color *tile, int w, int h, const War2_Tileset_Descriptor *ts, int img_nb);
typedef void (*War2_Sprites_Decode_Func)(const Pud_Color *sprite, int x, int y, int w, int h, const War2_Sprites_Descriptor *ts, int img_nb);

War2_Data *war2_open(const char *file, int verbose);
void war2_close(War2_Data *w2);
void war2_verbosity_set(War2_Data *w2, int level);

unsigned char *war2_entry_extract(War2_Data *w2, unsigned int entry, size_t *size_ret);
unsigned char *war2_palette_extract(War2_Data *w2, unsigned int entry);

War2_Tileset_Descriptor *war2_tileset_decode(War2_Data *w2, Pud_Era era, War2_Tileset_Decode_Func func);
void war2_tileset_descriptor_free(War2_Tileset_Descriptor *ts);

War2_Sprites_Descriptor *
war2_sprites_decode(War2_Data                *w2,
                    Pud_Player                player_color,
                    Pud_Era                   era,
                    unsigned int              object,
                    War2_Sprites_Decode_Func  func);


War2_Sprites_Descriptor *
war2_sprites_decode_entry(War2_Data *w2,
                          Pud_Player                player_color,
                          unsigned int              entry,
                          War2_Sprites_Decode_Func  func);

void war2_sprites_descriptor_free(War2_Sprites_Descriptor *ud);

Pud_Bool war2_png_write(const char          *file,
                        int                  w,
                        int                  h,
                        const unsigned char *data);

Pud_Bool
war2_jpeg_write(const char          *file,
                int                  w,
                int                  h,
                const unsigned char *data);


Pud_Bool
war2_ppm_write(const char          *file,
               int                  w,
               int                  h,
               const unsigned char *data);

void
war2_sprites_color_convert(Pud_Player     col,
                           unsigned char  in_r,
                           unsigned char  in_g,
                           unsigned char  in_b,
                           unsigned char *out_r,
                           unsigned char *out_g,
                           unsigned char *out_b);


#ifdef __cplusplus
%>
#endif
#endif /* ! _WAR2_H_ */

