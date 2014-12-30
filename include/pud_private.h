#ifndef _PRIVATE_H_
#define _PRIVATE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>

#include "../include/debug.h"
#include "../include/bool.h"
#include "../include/pud.h"
#include "../include/jpeg.h"

/* Visual hint when returning nothing */
#define VOID

#define PUD_VERBOSE(pud, lvl, msg, ...) \
   do { \
      if (pud->verbose >= lvl) { \
         fprintf(stdout, "-- " msg "\n", ## __VA_ARGS__); \
      } \
   } while (0)

#define PUD_SANITY_CHECK(pud, mode, ...) \
   do { \
      if (!(pud) || !((pud)->file)) { \
         DIE_RETURN(__VA_ARGS__, "Invalid PUD input [%p]", pud); \
      } \
      if (!(pud->open_mode & mode)) { \
         DIE_RETURN(__VA_ARGS__, "PUD open mode is [%i] expected [%i]", \
                    pud->open_mode, mode); \
      } \
   } while (0)

#define PUD_CHECK_FERROR(f, ret) \
   do { \
      if (ferror(f)) DIE_RETURN(ret, "Error while accessing file: %s", strerror(errno)); \
   } while (0)

#define _READ8(pud)  _read8(pud->file)
#define _READ16(pud) _read16(pud->file)
#define _READ32(pud) _read32(pud->file)

typedef struct _Color Color;

struct _Color
{
   unsigned char r;
   unsigned char g;
   unsigned char b;
};

//============================================================================//
//                                 Private API                                //
//============================================================================//

const char *long2bin(uint32_t x);
Color color_make(uint8_t r, uint8_t g, uint8_t b);
const char *dim2str(Pud_Dimensions dim);
const char *era2str(Pud_Era era);
Color color_for_player(uint8_t player);
const char *mode2str(Pud_Open_Mode mode);

Color pud_tile_to_color(Pud *pud, uint16_t tile);

bool pud_parse_type(Pud *pud);
bool pud_parse_ver(Pud *pud);
bool pud_parse_desc(Pud *pud);
bool pud_parse_ownr(Pud *pud);
bool pud_parse_side(Pud *pud);
bool pud_parse_era(Pud *pud);
bool pud_parse_dim(Pud *pud);
bool pud_parse_udta(Pud *pud);
bool pud_parse_alow(Pud *pud);
bool pud_parse_ugrd(Pud *pud);
bool pud_parse_sgld(Pud *pud);
bool pud_parse_slbr(Pud *pud);
bool pud_parse_soil(Pud *pud);
bool pud_parse_aipl(Pud *pud);
bool pud_parse_mtxm(Pud *pud);
bool pud_parse_sqm(Pud *pud);
bool pud_parse_oilm(Pud *pud);
bool pud_parse_regm(Pud *pud);
bool pud_parse_unit(Pud *pud);


#endif /* ! _PRIVATE_H_ */

