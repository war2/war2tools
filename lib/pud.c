#include "private.h"

static const char * const _sections[] =
{
   "TYPE", "VER ", "DESC", "OWNR", "ERA ",
   "ERAX", "DIM ", "UDTA", "ALOW", "UGRD",
   "SIDE", "SGLD", "SLBR", "SOIL", "AIPL",
   "MTXM", "SQM ", "OILM", "REGM", "UNIT"
};


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

   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, 0);
   if (sec > 19) DIE_RETURN(0, "Invalid section ID [%i]", sec);

   f = pud->file;
   sec_str = _sections[sec];

   /* If the section to search for is before the current section,
    * rewind the file to catch it. If it is after, do nothing */
   if (sec <= pud->current_section)
     rewind(f);

   /* Tell the PUD we are pointing at the last section.
    * In case of success the pud will be pointing at the section
    * it had to go.
    * On failure, it will have to rewind itself on next call */
   pud->current_section = PUD_SECTION_UNIT;

   fread(buf, sizeof(uint8_t), 4, f);
   PUD_CHECK_FERROR(f, 0);

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
        fread(&b, sizeof(uint8_t), 1, f);
        PUD_CHECK_FERROR(f, 0);
        buf[3] = b;
     }

   return 0;
}


Pud *
pud_open(const char    *file,
         Pud_Open_Mode  mode)
{
   Pud *pud;
   const char *m;

   if (file == NULL) DIE_RETURN(NULL, "NULL input file");

   switch (mode)
     {
      case PUD_OPEN_MODE_R:
         m = "rb";
         break;

      case PUD_OPEN_MODE_W:
         m = "wb";
         break;

      case PUD_OPEN_MODE_RW:
         m = "rb+";
         break;

      default:
         DIE_RETURN(NULL, "Wrong open mode [%i]", mode);
         break;
     }

   pud = calloc(1, sizeof(Pud));
   if (!pud) DIE_GOTO(err, "Failed to alloc Pud: %s", strerror(errno));

   pud->file = fopen(file, m);
   if (!pud->file) DIE_GOTO(err_free, "Failed to open file [%s] with mode [%s]",
                            file, m);

   pud->open_mode = mode;

   return pud;

err_free:
   free(pud);
err:
   return NULL;
}

void
pud_close(Pud *pud)
{
   if (!pud) return;
   fclose(pud->file);
   free(pud->units);
   free(pud->tiles_map);
   free(pud);
}

void
pud_verbose_set(Pud *pud,
                int  lvl)
{
   if (pud)
     pud->verbose = lvl;
}

bool
pud_parse(Pud *pud)
{
#define PARSE_SEC(sec) \
   if (!pud_parse_ ## sec(pud)) DIE_RETURN(false, "Failed to parse " #sec)

   PARSE_SEC(type);
   PARSE_SEC(ver);
   PARSE_SEC(desc);
   PARSE_SEC(ownr);
   PARSE_SEC(era); // Also parses ERAX
   PARSE_SEC(dim);
   PARSE_SEC(udta);
   PARSE_SEC(alow);
   PARSE_SEC(ugrd);
   PARSE_SEC(side);
   PARSE_SEC(sgld);
   PARSE_SEC(slbr);
   PARSE_SEC(soil);
   PARSE_SEC(aipl);
   PARSE_SEC(mtxm);
   PARSE_SEC(sqm);
   PARSE_SEC(oilm);
   PARSE_SEC(regm);
   PARSE_SEC(unit);

#undef PARSE_SEC

   return true;
}


/*============================================================================*
 *                              Output to images                              *
 *============================================================================*/


uint16_t
pud_tile_at(Pud *pud,
            int  x,
            int  y)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, 0);

   if (((unsigned int)(x * y)) >= (unsigned int)pud->tiles)
     DIE_RETURN(0, "Invalid coordinates %i,%i", x, y);

   return pud->tiles_map[y * pud->map_w + x];
}

void
pud_era_set(Pud     *pud,
            Pud_Era  era)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_W, VOID);
   pud->era = era;
}

void
pud_dimensions_set(Pud            *pud,
                   Pud_Dimensions  dims)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_W, VOID);

   size_t size;

   pud->dims = dims;
   pud_dimensions_to_size(dims, &(pud->map_w), &(pud->map_h));
   pud->tiles = pud->map_w * pud->map_h;

   size = pud->tiles * sizeof(uint16_t);

   /* Set by default light ground */
   pud->tiles_map = realloc(pud->tiles_map, size);
   if (!pud->tiles_map) DIE_RETURN(VOID, "Failed to allocate memory");
   memset(pud->tiles_map, 0x0050, size);

   pud->action_map = realloc(pud->action_map, size);
   if (!pud->action_map) DIE_RETURN(VOID, "Failed to allocate memory");
   memset(pud->action_map, 0, size);

   pud->movement_map = realloc(pud->movement_map, size);
   if (!pud->movement_map) DIE_RETURN(VOID, "Failed to allocate memory");
   memset(pud->movement_map, 0, size);
}

