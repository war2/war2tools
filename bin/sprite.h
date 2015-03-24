#ifndef _SPRITE_H_
#define _SPRITE_H_

typedef enum
{
   SPRITE_ORIENT_NORTH,
   SPRITE_ORIENT_NORTH_EAST,
   SPRITE_ORIENT_EAST,
   SPRITE_ORIENT_SOUTH_EAST,
   SPRITE_ORIENT_SOUTH
} Sprite_Orient;


Eina_Hash *sprite_hash_new(void);
unsigned char *sprite_get(Editor *ed, Pud_Unit unit, Sprite_Orient info, int *w, int *h);
Eet_File *sprite_buildings_open(Pud_Era era);
Eet_File *sprite_units_open(void);
void *sprite_load(Eet_File *src, const char *key, int *w_ret, int *h_ret);
Sprite_Orient sprite_orient_random_get(void);

#endif /* ! _SPRITE_H_ */

