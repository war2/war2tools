#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#define TEXTURE_WIDTH  32
#define TEXTURE_HEIGHT 32

struct _Texture_Dictionary_Entry
{
   unsigned int begin;
   unsigned int end;
};

struct _Texture_Dictionary
{
   Texture_Dictionary_Entry hwalls;  /* human walls */
   Texture_Dictionary_Entry owalls;  /* orc walls */
   Texture_Dictionary_Entry trees;
   Texture_Dictionary_Entry rocks;
   Texture_Dictionary_Entry nconstr; /* non constructible */
   Texture_Dictionary_Entry water_l; /* water limits */
   Texture_Dictionary_Entry constr; /* constructible */
   Texture_Dictionary_Entry constr_l; /* constructible limits */
   Texture_Dictionary_Entry water;
   Texture_Dictionary_Entry nconstr_x; /* extra non constructible */
   Texture_Dictionary_Entry constr_x; /* extra constructible */
};

Eina_Bool texture_init(void);
void texture_shutdown(void);

Eet_File *texture_tileset_open(Pud_Era era);

void *texture_load(Eet_File *src, unsigned int key);
unsigned char *texture_tile_access(Editor *ed, unsigned int x, unsigned int y);
unsigned char *texture_get(Editor *ed, unsigned int key);

void texture_dictionary_init(Texture_Dictionary *td, Pud_Era era);
unsigned int texture_dictionary_min(Editor *ed);
unsigned int texture_dictionary_max(Editor *ed);

unsigned int texture_dictionary_entry_random_get(Texture_Dictionary_Entry *entry);

Eina_Hash *texture_hash_new(void);


Eina_Bool
texture_water_is(Texture_Dictionary *dict,
                 unsigned int        tile);

Eina_Bool
texture_wall_is(Texture_Dictionary *dict,
                unsigned int        tile);

Eina_Bool
texture_tree_is(Texture_Dictionary *dict,
                 unsigned int        tile);

Eina_Bool
texture_rock_is(Texture_Dictionary *dict,
                 unsigned int        tile);

Eina_Bool
texture_walkable_is(Texture_Dictionary *dict,
                    unsigned int        tile);

#endif /* ! _TEXTURE_H_ */

