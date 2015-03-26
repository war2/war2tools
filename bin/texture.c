#include "war2edit.h"

void *
texture_load(Eet_File     *src,
             unsigned int  key,
             Eina_Bool    *missing)
{
   /* 4 channels (rgba) of 1 byte each */
   const int expected_size = TEXTURE_WIDTH * TEXTURE_HEIGHT * 4 * sizeof(unsigned char);
   void *mem;
   char key_str[8];
   int size;

   snprintf(key_str, sizeof(key_str), "0x%04x", key);

   /* Get raw data (uncompressed). */
   mem = eet_read(src, key_str, &size);
   if (!mem)
     {
        DBG("Cannot find key \"%s\"", key_str);
        /* Some tiles may not exist on some tilesets
         * Not finding the tile is likely not to be an error. */
        if (missing) *missing = EINA_TRUE;
        return NULL;
     }
   if (missing) *missing = EINA_FALSE;

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
            unsigned int  key,
            Eina_Bool    *missing)
{
   Eina_Bool chk;
   unsigned char *tex;

   tex = eina_hash_find(ed->textures, &key);
   if (tex == NULL)
     {
        tex = texture_load(ed->textures_src, key, missing);
        if (tex == NULL)
          {
             /* See texture_load() */
             if (EINA_LIKELY(missing && *missing))
               return NULL;
             else
               {
                  ERR("Failed to load texture for key [%u]", key);
                  return NULL;
               }
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
   return texture_get(ed, key, NULL);
}

Eina_Bool
texture_water_is(unsigned int tile)
{
   uint16_t t = tile & 0x00f0;
   return ((t == 0x10) || (t == 0x20));
}

Eina_Bool
texture_wall_is(unsigned int tile)
{
   return (((tile & 0x00f0) == 0xa0) || ((tile & 0x00f0) == 0xc0) ||
           ((tile & 0x0f00) == 0x900) || ((tile & 0x0f00) == 0x800));
}

Eina_Bool
texture_tree_is(unsigned int tile)
{
   return (((tile & 0x00f0) == 0x70) || ((tile & 0x0f00) == 0x700));
}

Eina_Bool
texture_rock_is(unsigned int tile)
{
   return (((tile & 0x00f0) == 0x80) || ((tile & 0x0f00) == 0x400));
}

Eina_Bool
texture_walkable_is(unsigned int tile)
{
   return (!texture_wall_is(tile) &&
           !texture_tree_is(tile) &&
           !texture_rock_is(tile));
}


