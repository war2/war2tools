#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#define TEXTURE_WIDTH  32
#define TEXTURE_HEIGHT 32


Eina_Bool texture_init(void);
void texture_shutdown(void);

Eet_File *texture_tileset_open(Pud_Era era);

void *texture_load(Eet_File *src, unsigned int key, Eina_Bool *missing);
unsigned char *texture_tile_access(Editor *ed, unsigned int x, unsigned int y);
unsigned char *texture_get(Editor *ed, unsigned int key, Eina_Bool *missing);

Eina_Hash *texture_hash_new(void);

Eina_Bool texture_water_is(unsigned int tile);
Eina_Bool texture_wall_is(unsigned int tile);
Eina_Bool texture_tree_is(unsigned int tile);
Eina_Bool texture_rock_is(unsigned int tile);
Eina_Bool texture_walkable_is(unsigned int tile);

#endif /* ! _TEXTURE_H_ */

