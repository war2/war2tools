#include "pudviewer.h"

#define PUD_VERBOSE(pud, msg, ...) \
   do { \
      if (pud->verbose) fprintf(stdout, "-- " msg "\n", ## __VA_ARGS__); \
   } while (0)

#define PUD_SANITY_CHECK(pud, ...) \
   do { \
      if (!(pud) || !((pud)->file)) { \
         DIE_RETURN(__VA_ARGS__, "Invalid PUD input [%p]", pud); \
      } \
   } while (0)

#define PUD_CHECK_FERROR(f, ret) \
   do { \
      if (ferror(f)) DIE_RETURN(ret, "Error while reading file"); \
   } while (0)


typedef struct _Color Color;

struct _Color
{
   unsigned char r;
   unsigned char g;
   unsigned char b;
};


struct _Pud
{
   FILE         *file;

   uint32_t      tag;
   uint16_t      version;
   char          description[32];
   uint16_t      era;
   Pud_Dimensions dims;

   struct {
      uint16_t players[8];
      uint16_t unusable[7];
      uint16_t neutral;
   } sgld, slbr, soil;

   struct _alow {
      uint32_t players[8];
      uint32_t unusable[7];
      uint32_t neutral;
   } unit_alow, spell_start, spell_alow, spell_acq, up_alow, up_acq;

   struct {
      uint8_t           time;
      uint16_t          gold;
      uint16_t          lumber;
      uint16_t          oil;
      uint16_t          icon;
      uint16_t          group;
      uint32_t          flags;
   } upgrade_cost[52];

   int map_x;
   int map_y;

   uint16_t *map_tiles;

   struct _unit {
      uint16_t x;
      uint16_t y;
      uint8_t  type;
      uint8_t  owner;
      uint16_t alter;
   } *units;

   int units_count;

// XXX Used by UDTA   struct {
// XXX Used by UDTA
// XXX Used by UDTA   } unit[110];

   Pud_Section   current_section;

   unsigned int  verbose       : 1;
   unsigned int  default_allow : 1;
};

static const char * const _sections[] =
{
   "TYPE", "VER ", "DESC", "OWNR", "ERA ",
   "ERAX", "DIM ", "UDTA", "ALOW", "UGRD",
   "SIDE", "SGLD", "SLBR", "SOIL", "AIPL",
   "MTXM", "SQM ", "OILM", "REGM", "UNIT"
};

static Color
_color_make(uint8_t r,
            uint8_t g,
            uint8_t b)
{
   Color c = {
      .r = r,
      .g = g,
      .b = b
   };
   return c;
}

static Color
_color_for_player(uint8_t player)
{
   switch (player)
     {
      case 0: return _color_make(0xc0, 0x00, 0x00); // Red
      case 1: return _color_make(0x00, 0x00, 0xc0); // Blue
      case 2: return _color_make(0x00, 0xff, 0x00); // Green
      case 3: return _color_make(0x80, 0x00, 0xc0); // Violet
      case 4: return _color_make(0xff, 0x80, 0x00); // Orange
      case 5: return _color_make(0x00, 0x00, 0x00); // Black
      case 6: return _color_make(0xff, 0xff, 0xff); // White
      case 7: return _color_make(0xff, 0xff, 0x00); // Yellow

      default:
              break;
     }

   return _color_make(0x7f, 0x7f, 0x7f);
}

static Color
_pud_tile_to_color(Pud      *pud,
                   uint16_t  tile)
{
   Color c;

   if (tile & 0x0010)
     {
        switch (pud->era)
          {
           case 0x00:
           case 0x04 ... 0xff:
              break;

           case 0x01:
              break;

           case 0x02:
              break;

           case 0x03:
              break;
          }
     }
   else if (tile & 0x0020)
     {
     }
   else if (tile & 0x0030)
     {
     }
   else if (tile & 0x0040)
     {
     }
   else if (tile & 0x0050)
     {
     }
   else if (tile & 0x0060)
     {
     }
   else if (tile & 0x0070)
     {
     }
   else if (tile & 0x0080)
     {
     }
   else if (tile & 0x0090)
     {
     }
   else if (tile & 0x00a0)
     {
     }
   else if (tile & 0x00b0)
     {
     }
   else if (tile & 0x00c0)
     {
     }

   return c;
}

bool
pud_section_is_optional(Pud_Section sec)
{
   return ((sec == PUD_SECTION_ERAX) ||
           (sec == PUD_SECTION_ALOW));
}

uint32_t
pud_go_to_section(Pud         *pud,
                  Pud_Section  sec)
{
   FILE *f;
   uint8_t b;
   uint32_t l;
   char buf[4];
   const char *sec_str;

   PUD_SANITY_CHECK(pud, 0);
   if (sec > 19) DIE_RETURN(0, "Invalid section ID [%i]", sec);

   f = pud->file;
   sec_str = _sections[sec];

   /* If the section to search for is before the current section,
    * rewind the file to catch it. If it is after, do nothing */
   if (sec <= pud->current_section)
     rewind(f);

   l = file_read_long(f);
   if (ferror(f)) DIE_RETURN(false, "Error while reading file");
   memcpy(buf, &l, sizeof(uint32_t));

   while (!feof(f))
     {
        if (!strncmp(buf, sec_str, 4))
          {
             /* Update current section */
             pud->current_section = sec;
             fread(&l, sizeof(uint32_t), 1, f);
             PUD_CHECK_FERROR(f, 0);
             return l;
          }

        memmove(buf, &(buf[1]), 3 * sizeof(char));
        b = file_read_byte(f);
        if (ferror(f)) DIE_RETURN(false, "Error while reading file");
        buf[3] = b;
     }
   pud->current_section = PUD_SECTION_UNIT; // Go to last section

   return 0;
}



Pud *
pud_new(const char *file)
{
   Pud *pud;

   pud = calloc(1, sizeof(Pud));
   if (!pud) DIE_GOTO(err, "Failed to alloc Pud: %s", strerror(errno));

   pud->file = fopen(file, "r");
   if (!pud->file) DIE_GOTO(err_free, "Failed to open file [%s]", file);

   return pud;

err_free:
   free(pud);
err:
   return NULL;
}

void
pud_free(Pud *pud)
{
   fclose(pud->file);
   free(pud->units);
   free(pud->map_tiles);
   free(pud);
}

void
pud_verbose_set(Pud *pud,
                int  on)
{
   if (pud)
     pud->verbose = !!on;
}

void
pud_print(Pud  *pud,
          FILE *stream)
{
   int i;

   if (!stream) stream = stdout;

   fprintf(stream, "Tag ID..........: %x\n", pud->tag);
   fprintf(stream, "Version.........: %x\n", pud->version);
   fprintf(stream, "Description.....: %s\n", pud->description);
   fprintf(stream, "Era.............: %x\n", pud->era);
   fprintf(stream, "Dimensions......: %x\n", pud->dims);

   //   fprintf(stream, "Units & Building allowed

   fprintf(stream, "Starting Gold...:\n");
   for (i = 0; i < 8; i++)
     fprintf(stream, "   player %i.....: %u\n", i + 1, pud->sgld.players[i]);
   for (i = 0; i < 7; i++)
     fprintf(stream, "   unusable %i...: %u\n", i + 1, pud->sgld.unusable[i]);
   fprintf(stream, "   neutral......: %u\n", pud->sgld.neutral);

   fprintf(stream, "Starting Lumber.:\n");
   for (i = 0; i < 8; i++)
     fprintf(stream, "   player %i.....: %u\n", i + 1, pud->slbr.players[i]);
   for (i = 0; i < 7; i++)
     fprintf(stream, "   unusable %i...: %u\n", i + 1, pud->slbr.unusable[i]);
   fprintf(stream, "   neutral......: %u\n", pud->slbr.neutral);

   fprintf(stream, "Starting Oil....:\n");
   for (i = 0; i < 8; i++)
     fprintf(stream, "   player %i.....: %u\n", i + 1, pud->soil.players[i]);
   for (i = 0; i < 7; i++)
     fprintf(stream, "   unusable %i...: %u\n", i + 1, pud->soil.unusable[i]);
   fprintf(stream, "   neutral......: %u\n", pud->soil.neutral);

}

void
pud_dimensions_to_size(Pud_Dimensions  dim,
                       int            *x_ret,
                       int            *y_ret)
{
   int x = 0, y = 0;

   switch (dim)
     {
      case PUD_DIMENSIONS_32_32:
         x = 32;
         y = 32;
         break;

      case PUD_DIMENSIONS_64_64:
         x = 64;
         y = 64;
         break;

      case PUD_DIMENSIONS_96_96:
         x = 96;
         y = 96;
         break;

      case PUD_DIMENSIONS_128_128:
         x = 128;
         y = 128;
         break;

      default:
         break;
     }

   if (x_ret) *x_ret = x;
   if (y_ret) *y_ret = y;
}

int
pud_parse(Pud *pud)
{
   pud_parse_type(pud);
   pud_parse_ver(pud);
   pud_parse_desc(pud);
   pud_parse_era(pud);
   pud_parse_dim(pud);
   pud_parse_udta(pud);
   pud_parse_alow(pud);
   pud_parse_ugrd(pud);
   pud_parse_sgld(pud);
   pud_parse_slbr(pud);
   pud_parse_soil(pud);
   pud_parse_mtxm(pud);
   pud_parse_oilm(pud);
   pud_parse_regm(pud);
   pud_parse_unit(pud);

  // pud_print(pud, stdout);


   return 0;
}

/*============================================================================*
 *                              Parsing Routines                              *
 *============================================================================*/

bool
pud_parse_type(Pud *pud)
{
   PUD_SANITY_CHECK(pud, 0);

   char buf[16];
   FILE *f = pud->file;
   uint32_t l;
   uint32_t chk;

   chk = pud_go_to_section(pud, PUD_SECTION_TYPE);
   if (!chk) DIE_RETURN(false, "Failed to reach section TYPE");
   PUD_VERBOSE(pud, "At section TYPE (size = %u)", chk);

   /* Read 10bytes + 2 unused */
   fread(buf, sizeof(uint8_t), 12, f);
   PUD_CHECK_FERROR(f, false);
   if (strncmp(buf, "WAR2 MAP\0\0", 10))
     DIE_RETURN(false, "TYPE section has a wrong header");

   /* Read ID TAG */
   l = file_read_long(f);
   PUD_CHECK_FERROR(f, false);

   pud->tag = l;

   return true;
}

bool
pud_parse_ver(Pud *pud)
{
   PUD_SANITY_CHECK(pud, false);

   FILE *f = pud->file;
   uint16_t w;
   uint32_t chk;

   chk = pud_go_to_section(pud, PUD_SECTION_VER);
   if (!chk) DIE_RETURN(false, "Failed to reach section VER");
   PUD_VERBOSE(pud, "At section VER (size = %u)", chk);

   fread(&w, sizeof(uint16_t), 1, f);
   PUD_CHECK_FERROR(f, false);

   pud->version = w;
   return true;
}

bool
pud_parse_desc(Pud *pud)
{
   PUD_SANITY_CHECK(pud, false);

   uint32_t chk;
   FILE *f = pud->file;
   char buf[32];

   chk = pud_go_to_section(pud, PUD_SECTION_DESC);
   if (!chk) DIE_RETURN(false, "Failed to reach section DESC");
   PUD_VERBOSE(pud, "At section DESC (size = %u)", chk);

   fread(buf, sizeof(char), 32, f);
   PUD_CHECK_FERROR(f, false);

   memcpy(pud->description, buf, 32 * sizeof(char));
   return true;
}

/*
 * TODO pud_parse_ownr()
 */

bool
pud_parse_era(Pud *pud)
{
   PUD_SANITY_CHECK(pud, false);

   uint32_t chk;
   FILE *f = pud->file;
   uint16_t w;

   chk = pud_go_to_section(pud, PUD_SECTION_ERAX);
   if (!chk) // Optional section, use ERA by default
     {
        PUD_VERBOSE(pud, "Failed to find ERAX. Trying with ERA...");
        chk = pud_go_to_section(pud, PUD_SECTION_ERA);
        if (!chk) DIE_RETURN(false, "Failed to reach section ERA");
        PUD_VERBOSE(pud, "At section ERA (size = %u)", chk);
     }

   fread(&w, sizeof(uint16_t), 1, f);
   PUD_CHECK_FERROR(f, false);

   pud->era = w;
   return true;
}

bool
pud_parse_dim(Pud *pud)
{
   PUD_SANITY_CHECK(pud, false);

   uint32_t chk;
   FILE *f = pud->file;
   uint16_t x, y;

   chk = pud_go_to_section(pud, PUD_SECTION_DIM);
   if (!chk) DIE_RETURN(false, "Failed to reach section DIM");
   PUD_VERBOSE(pud, "At section DIM (size = %u)", chk);

   fread(&x, sizeof(uint16_t), 1, f);
   fread(&y, sizeof(uint16_t), 1, f);
   PUD_CHECK_FERROR(f, false);

   if ((x == 32) && (y == 32))
     pud->dims = PUD_DIMENSIONS_32_32;
   else if ((x == 64) && (y == 64))
     pud->dims = PUD_DIMENSIONS_64_64;
   else if ((x == 96) && (y == 96))
     pud->dims = PUD_DIMENSIONS_96_96;
   else if ((x == 128) && (y == 128))
     pud->dims = PUD_DIMENSIONS_128_128;
   else
     return false;

   pud_dimensions_to_size(pud->dims, &pud->map_x, &pud->map_y);

   return true;
}

bool
pud_parse_udta(Pud *pud)
{
   PUD_SANITY_CHECK(pud, false);

   uint32_t chk;
   // FILE *f = pud->file;

   chk = pud_go_to_section(pud, PUD_SECTION_UDTA);
   if (!chk) DIE_RETURN(false, "Failed to reach section UDTA");
   PUD_VERBOSE(pud, "At section UDTA (size = %u)", chk);

   return false;
}

bool
pud_parse_alow(Pud *pud)
{
   PUD_SANITY_CHECK(pud, false);

   uint32_t chk;
   FILE *f = pud->file;
   uint32_t buf[16];
   struct _alow *ptrs[] = {
      &(pud->unit_alow),
      &(pud->spell_start),
      &(pud->spell_alow),
      &(pud->spell_acq),
      &(pud->up_alow),
      &(pud->up_acq)
   };
   const int ptrs_count = sizeof(ptrs) / sizeof(void *);
   int i;

   pud->default_allow = 0; // Reset before checking
   chk = pud_go_to_section(pud, PUD_SECTION_ALOW);
   if (!chk)
     {
        PUD_VERBOSE(pud, "Section ALOW (optional) not present. Skipping...");
        pud->default_allow = 1;
        return true;
     }
   PUD_VERBOSE(pud, "At section ALOW (size = %u)", chk);

   for (i = 0; i < ptrs_count; i++)
     {
        fread(buf, sizeof(uint32_t), 16, f);
        PUD_CHECK_FERROR(f, false);

        memcpy(&(ptrs[i]->players[0]),  &(buf[0]),  sizeof(uint32_t) * 8);
        memcpy(&(ptrs[i]->unusable[0]), &(buf[8]),  sizeof(uint32_t) * 7);
        memcpy(&(ptrs[i]->neutral),     &(buf[15]), sizeof(uint32_t) * 1);
     }

   return true;
}

bool
pud_parse_ugrd(Pud *pud)
{
   PUD_SANITY_CHECK(pud, false);

   uint32_t chk;
   FILE *f = pud->file;
   int i;
   uint8_t buf[208];

   chk = pud_go_to_section(pud, PUD_SECTION_UGRD);
   if (!chk) DIE_RETURN(false, "Failed to reach section UGRD");
   PUD_VERBOSE(pud, "At section UGRD (size = %u)", chk);

   for (i = 0; i < 52; i++)
     {
     }

   return true;
}

bool
pud_parse_sgld(Pud *pud)
{
   PUD_SANITY_CHECK(pud, false);

   uint32_t chk;
   FILE *f = pud->file;
   uint16_t buf[16];

   chk = pud_go_to_section(pud, PUD_SECTION_SGLD);
   if (!chk) DIE_RETURN(false, "Failed to reach section SGLD");
   PUD_VERBOSE(pud, "At section SGLD (size = %u)", chk);

   fread(buf, sizeof(uint16_t), 16, f);
   PUD_CHECK_FERROR(f, false);

   memcpy(&(pud->sgld.players[0]),  &(buf[0]),  sizeof(uint16_t) * 8);
   memcpy(&(pud->sgld.unusable[0]), &(buf[8]),  sizeof(uint16_t) * 7);
   memcpy(&(pud->sgld.neutral),     &(buf[15]), sizeof(uint16_t) * 1);

   return true;
}

bool
pud_parse_slbr(Pud *pud)
{
   PUD_SANITY_CHECK(pud, false);

   uint32_t chk;
   FILE *f = pud->file;
   uint16_t buf[16];

   chk = pud_go_to_section(pud, PUD_SECTION_SLBR);
   if (!chk) DIE_RETURN(false, "Failed to reach section SLBR");
   PUD_VERBOSE(pud, "At section SLBR (size = %u)", chk);

   fread(buf, sizeof(uint16_t), 16, f);
   PUD_CHECK_FERROR(f, false);

   memcpy(&(pud->slbr.players[0]),  &(buf[0]),  sizeof(uint16_t) * 8);
   memcpy(&(pud->slbr.unusable[0]), &(buf[8]),  sizeof(uint16_t) * 7);
   memcpy(&(pud->slbr.neutral),     &(buf[15]), sizeof(uint16_t) * 1);

   return true;
}

bool
pud_parse_soil(Pud *pud)
{
   PUD_SANITY_CHECK(pud, false);

   uint32_t chk;
   FILE *f = pud->file;
   uint16_t buf[16];

   chk = pud_go_to_section(pud, PUD_SECTION_SOIL);
   if (!chk) DIE_RETURN(false, "Failed to reach section SOIL");
   PUD_VERBOSE(pud, "At section SOIL (size = %u)", chk);

   fread(buf, sizeof(uint16_t), 16, f);
   PUD_CHECK_FERROR(f, false);

   memcpy(&(pud->soil.players[0]),  &(buf[0]),  sizeof(uint16_t) * 8);
   memcpy(&(pud->soil.unusable[0]), &(buf[8]),  sizeof(uint16_t) * 7);
   memcpy(&(pud->soil.neutral),     &(buf[15]), sizeof(uint16_t) * 1);

   return true;
}

bool
pud_parse_mtxm(Pud *pud)
{
   PUD_SANITY_CHECK(pud, false);

   uint32_t chk;
   FILE *f = pud->file;
   uint16_t w;
   int i, j, k = 0;

   chk = pud_go_to_section(pud, PUD_SECTION_MTXM);
   if (!chk) DIE_RETURN(false, "Failed to reach section MTXM");
   PUD_VERBOSE(pud, "At section MTXM (size = %u)", chk);

   pud->map_tiles = calloc(pud->map_x * pud->map_y, sizeof(uint16_t));
   if (!pud->map_tiles) DIE_RETURN(false, "Failed to allocate memory");

   for (i = 0; i < pud->map_x; i++)
     {
        for (j = 0; j < pud->map_y; j++)
          {
             fread(&w, sizeof(uint16_t), 1, f);
             PUD_CHECK_FERROR(f, false);
             pud->map_tiles[k++] = w;
          }
     }

   return true;
}

bool
pud_parse_oilm(Pud *pud)
{
   PUD_SANITY_CHECK(pud, false);

   uint32_t chk;

   chk = pud_go_to_section(pud, PUD_SECTION_OILM);
   if (!chk) PUD_VERBOSE(pud, "Section OILM (obsolete) not present. Skipping...");
   else PUD_VERBOSE(pud, "Section OILM (obsolete) present. Skipping...");

   return true;
}

bool
pud_parse_regm(Pud *pud)
{
   PUD_SANITY_CHECK(pud, false);

   uint32_t chk;
   FILE *f = pud->file;
   uint16_t w;

   chk = pud_go_to_section(pud, PUD_SECTION_REGM);
   if (!chk) DIE_RETURN(false, "Failed to reach section REGM");
   PUD_VERBOSE(pud, "At section REGM (size = %u)", chk);

   //   for (int i = 0; i < 128 * 128; i ++)
   //     {
   //   w = file_read_word(f);
   //   printf("-> %x\n", w);
   //     }
   //
   //   return true;

   return false;
}

bool
pud_parse_unit(Pud *pud)
{
   PUD_SANITY_CHECK(pud, false);

   uint32_t chk;
   FILE *f = pud->file;
   int i, units;
   struct _unit u;

   chk = pud_go_to_section(pud, PUD_SECTION_UNIT);
   if (!chk) DIE_RETURN(false, "Failed to reach section UNIT");
   PUD_VERBOSE(pud, "At section UNIT (size = %u)", chk);
   units = chk / 8;

   pud->units = calloc(units, sizeof(struct _unit));
   if (!pud->units) DIE_RETURN(false, "Failed to allocate memory");
   pud->units_count = units;

   fread(pud->units, sizeof(struct _unit), units, f);
   PUD_CHECK_FERROR(f, false);

   return true;
}

static unsigned char *
_minimap_bitmap_generate(Pud *pud,
                         int *size_ret)
{
   PUD_SANITY_CHECK(pud, NULL);

   unsigned char *map;
   struct _unit *u;
   Color c;
   int i;
   int idx;
   int size;

   size = pud->map_x * pud->map_y * 3;
   map = calloc(size, sizeof(unsigned char));
   if (!map) DIE_RETURN(NULL, "Failed to allocate memory");

   for (i = 0; i < pud->units_count; i++)
       {
          u = &(pud->units[i]);
          c = _color_for_player(u->owner);

          idx = ((u->y * pud->map_x) + u->x) * 3;

          map[idx + 0] = c.r;
          map[idx + 1] = c.g;
          map[idx + 2] = c.b;
       }

   if (size_ret) *size_ret = size;

   return map;
}

bool
pud_minimap_to_ppm(Pud        *pud,
                   const char *file)
{
   PUD_SANITY_CHECK(pud, false);

   FILE *f;
   int i, size;
   unsigned char *map;

   map = _minimap_bitmap_generate(pud, &size);
   if (!map) DIE_RETURN(false, "Failed to generate bitmap");

   f = fopen(file, "w");
   if (!f) DIE_RETURN(false, "Failed to open [%s]", file);
  
   /* Write PPM header */
   fprintf(f,
           "P3\n"
           "%i %i\n"
           "255\n",
           pud->map_x, pud->map_y);

   for (i = 0; i < size; i += 3)
     {
        fprintf(f, "%i %i %i\n", 
                map[i + 0],
                map[i + 1],
                map[i + 2]);
     }
   fclose(f);
   free(map);

   return true;
}

bool
pud_minimap_to_jpeg(Pud        *pud,
                    const char *file)
{
   PUD_SANITY_CHECK(pud, false);

   unsigned char *map;
   bool chk;

   map = _minimap_bitmap_generate(pud, NULL);
   if (!map) DIE_RETURN(false, "Failed to generate bitmap");

   chk = jpeg_write(file, pud->map_x, pud->map_y, map);
   free(map);

   return chk;
}

