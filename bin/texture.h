#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#define TEXTURE_WIDTH  32
#define TEXTURE_HEIGHT 32

GLuint texture_load(Evas_GL_API *api, const char *key);
Eina_Bool texture_tileset_select(Pud_Era era);
Eina_Bool texture_init(void);
void texture_shutdown(void);

#endif /* ! _TEXTURE_H_ */

