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

GLuint texture_load(Evas_GL_API *api, unsigned int key);
Eina_Bool texture_tileset_select(Pud_Era era);
Eina_Bool texture_init(void);
void texture_shutdown(void);
void texture_dictionary_init(Texture_Dictionary *td, Pud_Era era);
Eina_Hash *texture_hash_new(void);
GLuint texture_tile_access(Editor *ed, unsigned int x, unsigned int y);
GLuint texture_get(Editor *ed, unsigned int key);
void texture_hash_del(Editor *ed);
int texture_dictionary_min(Editor *ed);
int texture_dictionary_max(Editor *ed);

#endif /* ! _TEXTURE_H_ */

