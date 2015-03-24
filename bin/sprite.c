#include "war2edit.h"

void *
sprite_load(Eet_File   *src,
            const char *key,
            int        *w_ret,
            int        *h_ret)
{
   unsigned char *mem;
   int w, h, size, expected_size;

   mem = eet_read(src, key, &size);
   EINA_SAFETY_ON_NULL_RETURN_VAL(mem, NULL);

   memcpy(&w, mem + 0, sizeof(uint16_t));
   memcpy(&h, mem + 2, sizeof(uint16_t));

   expected_size = (w * h) + 4;
   if (expected_size != size)
     {
        CRI("Sprite data was loaded with size [%i], expected [%i]",
            size, expected_size);
        free(mem);
        return NULL;
     }

   DBG("Loaded sprite [%s] of size %ix%i\n", key, w, h);

   if (w_ret) *w_ret = w;
   if (h_ret) *h_ret = h;
   return mem + 4;
}

Eet_File *
sprite_units_open(void)
{
   Eet_File *ef;
   const char *file;

   file = DATA_DIR"/sprites/units/units.eet";
   ef = eet_open(file, EET_FILE_MODE_READ);
   if (EINA_UNLIKELY(ef == NULL))
     {
        CRI("Failed to open [%s]", file);
        return NULL;
     }
   DBG("Open units file [%s]", file);

   return ef;
}

Eet_File *
sprite_buildings_open(Pud_Era era)
{
   Eet_File *ef;
   const char *file;

   switch (era)
     {
      case PUD_ERA_FOREST:    file = DATA_DIR"/sprites/buildings/forest.eet";    break;
      case PUD_ERA_WINTER:    file = DATA_DIR"/sprites/buildings/winter.eet";    break;
      case PUD_ERA_WASTELAND: file = DATA_DIR"/sprites/buildings/wasteland.eet"; break;
      case PUD_ERA_SWAMP:     file = DATA_DIR"/sprites/buildings/swamp.eet";     break;
     }

   ef = eet_open(file, EET_FILE_MODE_READ);
   if (EINA_UNLIKELY(ef == NULL))
     {
        CRI("Failed to open [%s]", file);
        return NULL;
     }
   DBG("Open buildings file [%s]", file);

   return ef;
}

unsigned char *
sprite_get(Editor      *ed,
           Pud_Unit     unit,
           Sprite_Info  info,
           int         *w,
           int         *h)
{
   unsigned char *data;
   char key[64];
   Eet_File *ef;
   Eina_Bool chk;
   int len;

   if (info == SPRITE_INFO_BUILDING)
     {
        ef = ed->buildings;
        len = snprintf(key, sizeof(key), "%s", pud_unit2str(unit));
     }
   else
     {
        ef = ed->units;
        if ((unit == PUD_UNIT_GNOMISH_SUBMARINE) ||
            (unit == PUD_UNIT_GIANT_TURTLE))
          {
             len = snprintf(key, sizeof(key), "%s/%s/%i",
                      pud_unit2str(unit), ed->era_str, info - 1);
          }
        else
          {
             len = snprintf(key, sizeof(key), "%s/%i",
                      pud_unit2str(unit), info - 1);
          }
     }

   data = eina_hash_find(ed->sprites, key);
   if (data == NULL)
     {
        data = sprite_load(ef, key, w, h);
        if (EINA_UNLIKELY(data == NULL))
          {
             ERR("Failed to load sprite for key [%s]", key);
             return NULL;
          }
        chk = eina_hash_add(ed->sprites, key, data);
        if (chk == EINA_FALSE)
          {
             ERR("Failed to add sprite <%p> to hash", data);
             free(data);
             return NULL;
          }
        DBG("Access key [%s] (not yet registered). SRT = <%p>", key, data);
        return data;
     }
   else
     {
        if (w) memcpy(w, data - 4, sizeof(uint16_t));
        if (h) memcpy(h, data - 2, sizeof(uint16_t));
        DBG("Access key [%s] (already registered). SRT = <%p>", key, data);
        return data;
     }
}

static void
_free_cb(void *data)
{
   /* There is a 4 bytes offset */
   data -= 4;
   free(data);
}

Eina_Hash *
sprite_hash_new(void)
{
   return eina_hash_string_superfast_new(_free_cb);
}

