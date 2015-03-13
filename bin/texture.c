#include "war2edit.h"

static Eet_File *_ef = NULL;

GLuint
texture_load(Evas_GL_API  *api,
             unsigned int  key)
{
   /* 4 channels (rgba) of 1 byte each */
   const int expected_size = TEXTURE_WIDTH * TEXTURE_HEIGHT * 4 * sizeof(unsigned char);
   GLuint tid;
   void *mem;
   int size;
   char key_str[8];

   snprintf(key_str, sizeof(key_str), "%u", key);

   /* Get raw data (uncompressed). The image is pre-flipped vertically. */
   mem = eet_read(_ef, key_str, &size);
   EINA_SAFETY_ON_NULL_RETURN_VAL(mem, 0);

   /* Check the size */
   if (EINA_UNLIKELY(size != expected_size))
     {
        CRI("Image raw data was loaded with size [%i], expected [%i].",
            size, expected_size);
        free(mem);
        return 0;
     }

   /* Configure texture */
   api->glGenTextures(1, &tid);
   api->glBindTexture(GL_TEXTURE_2D, tid);
   api->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   api->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   api->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   api->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   api->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXTURE_WIDTH, TEXTURE_HEIGHT,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, mem);
   api->glBindTexture(GL_TEXTURE_2D, 0);
   DBG("Loaded texture [%s] with ID [%i]", key_str, tid);

   return tid;
}

Eina_Bool
texture_tileset_select(Pud_Era era)
{
   const char *file;

   switch (era)
     {
      case PUD_ERA_FOREST:    file = "../data/tiles/eet/forest.eet";    break;
      case PUD_ERA_WINTER:    file = "../data/tiles/eet/winter.eet";    break;
      case PUD_ERA_WASTELAND: file = "../data/tiles/eet/wasteland.eet"; break;
      case PUD_ERA_SWAMP:     file = "../data/tiles/eet/swamp.eet";     break;
     }

   if (_ef) eet_close(_ef);
   _ef = eet_open(file, EET_FILE_MODE_READ);
   EINA_SAFETY_ON_NULL_RETURN_VAL(_ef, EINA_FALSE);

   return EINA_TRUE;
}

Eina_Bool
texture_init(void)
{
   return EINA_TRUE;
}

void
texture_shutdown(void)
{
   if (_ef)
     {
        eet_close(_ef);
        _ef = NULL;
     }
}

#define TD_FILL(td,d2,d3,d4,d5,d6,d7,d8,d9,d10,d11,d12,d13,d14,d15,d16,d17,d18,d19,d20,d21,d22,d23) \
   do { \
      td->hwalls.begin  = d2; \
      td->hwalls.end = d3; \
      td->owalls.begin = d4; \
      td->owalls.end = d5; \
      td->trees.begin = d6; \
      td->trees.end = d7; \
      td->rocks.begin = d8; \
      td->rocks.end = d9; \
      td->nconstr.begin = d10; \
      td->nconstr.end = d11; \
      td->water_l.begin = d12; \
      td->water_l.end = d13; \
      td->constr.begin = d14; \
      td->constr.end = d15; \
      td->constr_l.begin = d16; \
      td->constr_l.end = d17; \
      td->water.begin = d18; \
      td->water.end = d19; \
      td->nconstr_x.begin = d20; \
      td->nconstr_x.end = d21; \
      td->constr_x.begin = d22; \
      td->constr_x.end = d23; \
   } while (0)

void
texture_dictionary_init(Texture_Dictionary *td,
                        Pud_Era             era)
{
   EINA_SAFETY_ON_NULL_RETURN(td);

   switch (era)
     {
      case PUD_ERA_FOREST:
         TD_FILL(td, 17, 34, 35, 52, 103, 142, 143, 180, 181, 206, 207, 238,
                 239, 270, 271, 300, 301, 334, 335, 356, 357, 372);
         break;

      case PUD_ERA_WINTER:
         TD_FILL(td, 17, 34, 35, 52, 103, 137, 138, 175, 176, 199, 200, 231,
                 232, 259, 260, 291, 292, 331, 332, 349, 350, 379);
         break;

      case PUD_ERA_WASTELAND:
         TD_FILL(td, 17, 34, 35, 52, 103, 139, 140, 177, 178, 201, 202, 233,
                 234, 264, 265, 296, 297, 330, 331, 346, 347, 373);
         break;

      case PUD_ERA_SWAMP:
         TD_FILL(td, 17, 34, 35, 52, 103, 137, 138, 175, 176, 207, 208, 239,
                 240, 270, 271, 302, 303, 336, 337, 354, 355, 389);
         break;
     }
}

#undef TD_FILL

static void
_hash_free_cb(void *data)
{
   GLuint *tid = data;
   // FIXME call glDeleteTextures()
   free(tid);
}

Eina_Hash *
texture_hash_new(void)
{
   return eina_hash_int32_new(_hash_free_cb);
}

void
texture_hash_del(Editor *ed)
{
   EINA_SAFETY_ON_NULL_RETURN(ed);
   eina_hash_free(ed->tex.hash);
}

GLuint
texture_get(Editor       *ed,
            unsigned int  key)
{
   EINA_SAFETY_ON_TRUE_RETURN_VAL((key < (unsigned int )texture_dictionary_min(ed)) ||
                                  (key > (unsigned int )texture_dictionary_max(ed)), 0);

   GLuint tid;
   GLuint *tid_ptr;
   Eina_Bool chk;

   tid_ptr = eina_hash_find(ed->tex.hash, &key);
   if (tid_ptr == NULL)
     {
        tid_ptr = malloc(sizeof(GLuint));
        EINA_SAFETY_ON_NULL_RETURN_VAL(tid_ptr, 0);

        tid = texture_load(ed->gl.api, key);
        if (EINA_UNLIKELY(tid == 0))
          {
             ERR("Failed to load texture for key [%u]", key);
             free(tid_ptr);
             return 0;
          }
        *tid_ptr = tid;
        chk = eina_hash_add(ed->tex.hash, &key, tid_ptr);
        if (chk == EINA_FALSE)
          {
             ERR("Failed to add texture id [%u] to hash", tid);
             free(tid_ptr);
             return 0;
          }
        DBG("Access key: [%u] (not yet registered). TID = %u", key, tid);
        return tid;
     }
   else
     {
        tid = *tid_ptr;
        DBG("Access key: [%u] (already registered). TID = %u", key, tid);
        return tid;
     }
}

GLuint
texture_tile_access(Editor       *ed,
                    unsigned int  x,
                    unsigned int  y)
{
   unsigned int key;

   key = ed->cells[y][x].tile;
   return texture_get(ed, key);
}

int
texture_dictionary_min(Editor *ed)
{
   return ed->tex.dict.hwalls.begin;
}

int
texture_dictionary_max(Editor *ed)
{
   return ed->tex.dict.constr_x.end;
}

