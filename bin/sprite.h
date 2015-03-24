#ifndef _SPRITE_H_
#define _SPRITE_H_

typedef enum
{
   SPRITE_INFO_NORTH            = 0,
   SPRITE_INFO_NORTH_EAST       = 1,
   SPRITE_INFO_EAST             = 2,
   SPRITE_INFO_SOUTH_EAST       = 3,
   SPRITE_INFO_SOUTH            = 4,
   SPRITE_INFO_SOUTH_WEST,
   SPRITE_INFO_WEST,
   SPRITE_INFO_NORTH_WEST,
   SPRITE_INFO_ICON , /* special value for icons */
} Sprite_Info;


Eina_Hash *sprite_hash_new(void);
unsigned char *sprite_get(Editor *ed, Pud_Unit unit, Sprite_Info info, int *w, int *h, Eina_Bool *flip_me);
Eet_File *sprite_buildings_open(Pud_Era era);
Eet_File *sprite_units_open(void);
void *sprite_load(Eet_File *src, const char *key, int *w_ret, int *h_ret);
Sprite_Info sprite_info_random_get(void);

#endif /* ! _SPRITE_H_ */

