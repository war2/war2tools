#include "war2edit.h"

void *
texture_load(Eet_File     *src,
             unsigned int  key)
{
   /* 4 channels (rgba) of 1 byte each */
   const int expected_size = TEXTURE_WIDTH * TEXTURE_HEIGHT * 4 * sizeof(unsigned char);
   void *mem;
   char key_str[8];
   int size;

   snprintf(key_str, sizeof(key_str), "%u", key);

   /* Get raw data (uncompressed). The image is pre-flipped vertically. */
   mem = eet_read(src, key_str, &size);
   EINA_SAFETY_ON_NULL_RETURN_VAL(mem, NULL);

   /* Check the size */
   if (EINA_UNLIKELY(size != expected_size))
     {
        CRI("Image raw data was loaded with size [%i], expected [%i].",
            size, expected_size);
        free(mem);
        return NULL;
     }

   DBG("Loaded texture [%s] at <%p>", key_str, mem);

   return mem;
}

Eet_File *
texture_tileset_open(Pud_Era era)
{
   Eet_File *ef;
   const char *file;

   switch (era)
     {
      case PUD_ERA_FOREST:    file = DATA_DIR"/tiles/forest.eet";    break;
      case PUD_ERA_WINTER:    file = DATA_DIR"/tiles/winter.eet";    break;
      case PUD_ERA_WASTELAND: file = DATA_DIR"/tiles/wasteland.eet"; break;
      case PUD_ERA_SWAMP:     file = DATA_DIR"/tiles/swamp.eet";     break;
     }

   ef = eet_open(file, EET_FILE_MODE_READ);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ef, NULL);
   DBG("Open tileset file [%s]", file);

   return ef;
}

Eina_Bool
texture_init(void)
{
   return EINA_TRUE;
}

void
texture_shutdown(void)
{
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
   free(data);
}

Eina_Hash *
texture_hash_new(void)
{
   return eina_hash_int32_new(_hash_free_cb);
}

unsigned char *
texture_get(Editor       *ed,
            unsigned int  key)
{
   EINA_SAFETY_ON_TRUE_RETURN_VAL(((int)key < texture_dictionary_min(ed)) ||
                                  ((int)key > texture_dictionary_max(ed)),
                                  NULL);

   Eina_Bool chk;
   unsigned char *tex;

   tex = eina_hash_find(ed->textures, &key);
   if (tex == NULL)
     {
        tex = texture_load(ed->textures_src, key);
        if (EINA_UNLIKELY(tex == NULL))
          {
             ERR("Failed to load texture for key [%u]", key);
             return NULL;
          }
        chk = eina_hash_add(ed->textures, &key, tex);
        if (chk == EINA_FALSE)
          {
             ERR("Failed to add texture <%p> to hash", tex);
             free(tex);
             return NULL;
          }
        DBG("Access key: [%u] (not yet registered). TEX = <%p>", key, tex);
        return tex;
     }
   else
     {
        DBG("Access key: [%u] (already registered). TEX = <%p>", key, tex);
        return tex;
     }
}

unsigned char *
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
   return ed->tex_dict.hwalls.begin;
}

int
texture_dictionary_max(Editor *ed)
{
   return ed->tex_dict.constr_x.end;
}

