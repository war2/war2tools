#ifndef _WAR2_PRIVATE_H_
#define _WAR2_PRIVATE_H_

#include "war2.h"
#include "pud_private.h"

static inline uint32_t
_read32(FILE *f)
{
   uint32_t l;
   fread(&l, sizeof(uint32_t), 1, f);
   return l;
}

static inline uint16_t
_read16(FILE *f)
{
   uint16_t w;
   fread(&w, sizeof(uint16_t), 1, f);
   return w;
}

static inline uint8_t
_read8(FILE *f)
{
   uint8_t b;
   fread(&b, sizeof(uint8_t), 1, f);
   return b;
}

/* Because defined in pud_private.h */
#undef _READ8
#undef _READ16
#undef _READ32

#define _READ8(w2)  _read8(w2->file)
#define _READ16(w2) _read16(w2->file)
#define _READ32(w2) _read32(w2->file)

#define WAR2_VERBOSE(w2, lvl, msg, ...) \
   do { \
      if (w2->verbose >= lvl) { \
         fprintf(stdout, "-- " msg "\n", ## __VA_ARGS__); \
      } \
   } while (0)

#endif /* ! _WAR2_PRIVATE_H_ */

