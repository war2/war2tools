#include "war2edit.h"

static Eet_File *_ef = NULL;

GLuint
texture_load(Evas_GL_API *api,
             const char  *key)
{
   /* 4 channels (rgba) of 1 byte each */
   const int expected_size = TEXTURE_WIDTH * TEXTURE_HEIGHT * 4 * sizeof(unsigned char);
   GLuint tid;
   void *mem;
   int size;

   /* Get raw data (uncompressed) */
   mem = eet_read(_ef, key, &size);
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
   DBG("Loaded texture [%s] with ID [%i]", key, tid);

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

