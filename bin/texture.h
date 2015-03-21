#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#define TEXTURE_WIDTH  32
#define TEXTURE_HEIGHT 32

struct _Texture_Dictionary
{
   struct {
      unsigned short begin;
      unsigned short end;
   }  hwalls,
      owalls,
      trees,
      rocks,
      nconstr,
      water_l,
      constr,
      constr_l,
      water,
      nconstr_x,
      constr_x;
};

Eina_Bool texture_init(void);
void texture_shutdown(void);

Eet_File *texture_tileset_open(Pud_Era era);

void *texture_load(Eet_File *src, unsigned int key);
unsigned char *texture_tile_access(Editor *ed, unsigned int x, unsigned int y);
unsigned char *texture_get(Editor *ed, unsigned int key);

void texture_dictionary_init(Texture_Dictionary *td, Pud_Era era);
int texture_dictionary_min(Editor *ed);
int texture_dictionary_max(Editor *ed);

Eina_Hash *texture_hash_new(void);

#endif /* ! _TEXTURE_H_ */

