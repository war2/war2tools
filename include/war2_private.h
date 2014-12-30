#ifndef _WAR2_PRIVATE_H_
#define _WAR2_PRIVATE_H_

#include "war2.h"
#include "pud_private.h"

static inline uint32_t
_read32(War2_Data *w2)
{
   uint32_t l;
   fread(&l, sizeof(uint32_t), 1, w2->file);
   return l;
}

static inline uint16_t
_read16(War2_Data *w2)
{
   uint16_t w;
   fread(&w, sizeof(uint16_t), 1, w2->file);
   return w;
}

static inline uint8_t
_read8(War2_Data *w2)
{
   uint8_t b;
   fread(&b, sizeof(uint8_t), 1, w2->file);
   return b;
}

#define WAR2_VERBOSE(w2, lvl, msg, ...) \
   do { \
      if (w2->verbose >= lvl) { \
         fprintf(stdout, "-- " msg "\n", ## __VA_ARGS__); \
      } \
   } while (0)

#endif /* ! _WAR2_PRIVATE_H_ */

