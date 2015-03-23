#ifndef _SPRITE_H_
#define _SPRITE_H_

typedef enum
{
   SPRITE_INFO_BUILDING = 0,
   SPRITE_INFO_NORTH,
   SPRITE_INFO_NORTH_EAST,
   SPRITE_INFO_EAST,
   SPRITE_INFO_SOUTH_EAST,
   SPRITE_INFO_SOUTH
} Sprite_Info;


Eina_Hash *sprite_hash_new(void);
unsigned char *sprite_get(Editor *ed, Pud_Unit unit, Sprite_Info info, int *w, int *h);
Eet_File *sprite_buildings_open(Pud_Era era);
Eet_File *sprite_units_open(void);
void *sprite_load(Eet_File *src, const char *key, int *w_ret, int *h_ret);

#endif /* ! _SPRITE_H_ */

