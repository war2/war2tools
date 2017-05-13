/*
 * Copyright (c) 2014-2016 Jean Guyomarc'h
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "common.h"
#include "pud_private.h"

static const char * const _pud_sections[] =
{
   "TYPE", "VER ", "DESC", "OWNR", "ERA ",
   "ERAX", "DIM ", "UDTA", "ALOW", "UGRD",
   "SIDE", "SGLD", "SLBR", "SOIL", "AIPL",
   "MTXM", "SQM ", "OILM", "REGM", "UNIT"
};

PUDAPI const char *
pud_section_to_string(Pud_Section section)
{
   return ((unsigned) section >= 20) ? NULL : _pud_sections[section];
}

PUDAPI Pud_Bool
pud_section_has(const Pud   *pud,
                Pud_Section  section)
{
   if (!pud) return PUD_FALSE;
   return !!(pud->private_data->sections & (1 << section));
}

PUDAPI Pud_Bool
pud_section_valid_is(const char *sec)
{
   unsigned int i;

   if (!sec) return PUD_FALSE;
   for (i = 0; i < sizeof(_pud_sections) / sizeof(_pud_sections[0]); i++)
     {
        if (!strncmp(sec, _pud_sections[i], 4))
          return PUD_TRUE;
     }

   return PUD_FALSE;
}

PUDAPI Pud_Bool
pud_init(void)
{
   srand(time(NULL));
   return PUD_TRUE;
}

PUDAPI void
pud_shutdown(void)
{
   /* Nothing to do */
}

/*
 * TODO FIXME
 * Bad design (this file should not have been parsed like this)
 */
PUDAPI_INTERNAL uint32_t
pud_go_to_section(Pud         *pud,
                  Pud_Section  sec)
{
   uint8_t b;
   uint32_t l;
   char buf[4];
   const char *sec_str;

   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, 0);
   if (sec > 19) DIE_RETURN(0, "Invalid section ID [%i]", sec);

   sec_str = _pud_sections[sec];

   /* If the section to search for is before the current section,
    * rewind the file to catch it. If it is after, do nothing */
   if (sec <= pud->private_data->current_section)
     pud->private_data->ptr = pud->private_data->mem_map;

   /* Tell the PUD we are pointing at the last section.
    * In case of success the pud will be pointing at the section
    * it had to go.
    * On failure, it will have to rewind itself on next call */
   pud->private_data->current_section = PUD_SECTION_UNIT;

   READBUF(pud, buf, char, 4, FAIL(0));

   while (pud_mem_map_ok(pud))
     {
        if (!strncmp(buf, sec_str, 4))
          {
             /* Update current section */
             pud->private_data->current_section = sec;

             l = READ32(pud, FAIL(0));
             return l;
          }

        memmove(buf, &(buf[1]), 3 * sizeof(char));
        b = READ8(pud, FAIL(0));
        buf[3] = b;
     }

   return 0;
}

PUDAPI uint32_t
pud_tag_generate(void)
{
   return rand() % UINT32_MAX;
}

static Pud_Private *
_private_new(void)
{
   return calloc(1, sizeof(Pud_Private));
}

static void
_private_free(Pud_Private *priv)
{
   if (priv)
     {
        if (priv->mem_map)
          common_file_munmap(priv->mem_map, priv->mem_map_size);
        free(priv);
     }
}

PUDAPI Pud *
pud_open(const char    *file,
         Pud_Open_Mode  mode)
{
   Pud *pud;

   /* RST memory */
   pud = calloc(1, sizeof(Pud));
   if (!pud) DIE_GOTO(err, "Failed to alloc Pud: %s", strerror(errno));

   pud->private_data = _private_new();
   if (!pud->private_data) DIE_GOTO(err, "Failed to create private structure");

   /* Keep the open mode around */
   pud->private_data->open_mode = mode;

   /*
    * +) If the file exists, and we are allowed to read from it,
    * map it. Unless we have specified the NO_PARSE flag, parse
    * it automatically.
    *
    * +) If the file is nonexistant, create it if write access
    * was granted
    */
   if (common_file_exists(file))
     {
        if (mode & PUD_OPEN_MODE_R)
          {
             pud->private_data->mem_map = common_file_mmap(file, &pud->private_data->mem_map_size);
             if (!pud->private_data->mem_map) DIE_GOTO(err, "Failed to map file \"%s\"", file);
             pud->private_data->ptr = pud->private_data->mem_map;

             if (!(mode & PUD_OPEN_MODE_NO_PARSE))
               {
                  if (!pud_parse(pud))
                    DIE_GOTO(err, "Failed to parse pud file \"%s\"", file);
               }
          }
     }
   else
     {
        if (mode & PUD_OPEN_MODE_W)
          {
             /*
              * Generate default PUD
              */
             if (!pud_defaults_set(pud))
               DIE_GOTO(err, "Failed to set defaults");

             pud->tag = pud_tag_generate();
             pud_version_set(pud, PUD_VERSION_WAR2);
             pud_description_set(pud, "");
             pud_era_set(pud, PUD_ERA_FOREST);
             pud_dimensions_set(pud, PUD_DIMENSIONS_32_32);
          }
        else
          DIE_GOTO(err, "Cannot READ %s that does not exist", file);
     }

   return pud;

err:
   pud_close(pud);
   return NULL;
}

PUDAPI void
pud_close(Pud *pud)
{
   if (!pud) return;
   _private_free(pud->private_data);
   free(pud->units);
   free(pud->tiles_map);
   free(pud->action_map);
   free(pud->movement_map);
   free(pud->oil_map);
   free(pud);
}

PUDAPI Pud_Bool
pud_parse(Pud *pud)
{
   pud->private_data->sections = 0;

#define PARSE_SEC(sec) \
   if (!pud_parse_ ## sec(pud)) DIE_RETURN(PUD_FALSE, "Failed to parse " #sec)

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

   /* Is assumed valid */
   pud->private_data->init = 1;

   return PUD_TRUE;
}

PUDAPI void
pud_era_set(Pud     *pud,
            Pud_Era  era)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_W, VOID);
   pud->era = era;
}

PUDAPI Pud_Bool
pud_dimensions_set(Pud            *pud,
                   Pud_Dimensions  dims)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_W, PUD_FALSE);

   size_t size;
   unsigned int i, tiles;
   void *ptr;

   pud_dimensions_to_size(dims, &(pud->map_w), &(pud->map_h));
   tiles = pud->map_w * pud->map_h;

   size = tiles * sizeof(uint16_t);

   /* Set by default light ground */
   ptr = realloc(pud->tiles_map, size);
   if (!ptr) DIE_RETURN(PUD_FALSE, "Failed to allocate memory");
   pud->tiles_map = ptr;
   for (i = 0; i < tiles; i++)
     pud->tiles_map[i] = 0x0050;

   ptr = realloc(pud->action_map, size);
   if (!ptr) DIE_RETURN(PUD_FALSE, "Failed to allocate memory");
   pud->action_map = ptr;
   memset(pud->action_map, 0, size);

   ptr = realloc(pud->movement_map, size);
   if (!ptr) DIE_RETURN(PUD_FALSE, "Failed to allocate memory");
   pud->movement_map = ptr;
   memset(pud->movement_map, 0, size);

   pud->tiles = tiles;
   pud->dims = dims;
   return PUD_TRUE;
}

PUDAPI Pud_Bool
pud_write(const Pud  *pud,
          const char *file)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_W, PUD_FALSE);
   if (!file) DIE_RETURN(PUD_FALSE, "Cannot write in NULL file");

   const Pud *p = pud; // Shortcut
   FILE *f;
   uint8_t b;
   uint16_t w;
   uint32_t l;
   unsigned int i, j, map_len, units_len;

   map_len = p->tiles * sizeof(uint16_t);
   units_len = p->units_count * sizeof(Pud_Unit_Info);

   f = fopen(file, "wb");
   if (!f) DIE_RETURN(PUD_FALSE, "Failed to open [%s]", file);
   setvbuf(f, NULL, _IOFBF, 0); /* Fully buffered */

#define W8(val, nb) \
   do { \
      for (j = 0; j < nb; j++) fwrite(&(val), sizeof(uint8_t), 1, f); \
      PUD_CHECK_FERROR(f, PUD_FALSE); \
   } while (0)

#define W16(val, nb) \
   do { \
      for (j = 0; j < nb; j++) fwrite(&(val), sizeof(uint16_t), 1, f); \
      PUD_CHECK_FERROR(f, PUD_FALSE); \
   } while (0)

#define W32(val, nb) \
   do { \
      for (j = 0; j < nb; j++) fwrite(&(val), sizeof(uint32_t), 1, f); \
      PUD_CHECK_FERROR(f, PUD_FALSE); \
   } while (0)

#define WI8(imm, nb) \
   do { \
      b = imm; \
      for (j = 0; j < nb; j++) fwrite(&(b), sizeof(uint8_t), 1, f); \
      PUD_CHECK_FERROR(f, PUD_FALSE); \
   } while (0)

#define WI16(imm, nb) \
   do { \
      w = imm; \
      for (j = 0; j < nb; j++) fwrite(&(w), sizeof(uint16_t), 1, f); \
      PUD_CHECK_FERROR(f, PUD_FALSE); \
   } while (0)

#define WI32(imm, nb) \
   do { \
      l = imm; \
      for (j = 0; j < nb; j++) fwrite(&(l), sizeof(uint32_t), 1, f); \
      PUD_CHECK_FERROR(f, PUD_FALSE); \
   } while (0)

#define WSEC(sec, len) \
   do { \
      fwrite(_pud_sections[sec], sizeof(int8_t), 4, f); \
      PUD_CHECK_FERROR(f, PUD_FALSE); \
      W32(len, 1); \
   } while (0)

#define WISEC(sec, len) \
   do { \
      fwrite(_pud_sections[sec], sizeof(int8_t), 4, f); \
      PUD_CHECK_FERROR(f, PUD_FALSE); \
      WI32(len, 1); \
   } while (0)

#define WSTR(str) \
   do { \
      fwrite(str, sizeof(int8_t), sizeof(str) - 1, f); \
      PUD_CHECK_FERROR(f, PUD_FALSE); \
   } while (0)

   /* Section TYPE */
   WISEC(PUD_SECTION_TYPE, 16);
   WSTR("WAR2 MAP");
   WI8(0, 2);           // Unused
   WI8(0x0a, 1);
   WI8(0xff, 1);
   W32(p->tag, 1);

   /* Section VER */
   WISEC(PUD_SECTION_VER, 2);
   WI16(p->version, 1);  // Version

   /* Section DESC */
   WISEC(PUD_SECTION_DESC, 32);
   for (i = 0; i < 32; ++i) W8(p->description[i], 1);

   /* Section OWNR */
   WISEC(PUD_SECTION_OWNR, 16);
   for (i = 0; i < 8; i++) W8(p->owner.players[i], 1);
   for (i = 0; i < 7; i++) W8(p->owner.unusable[i], 1);
   W8(p->owner.neutral, 1);

   /* Section ERA */
   WISEC(PUD_SECTION_ERA, 2);
   W16(p->era, 1);

   /* Section ERAX */
   if (p->private_data->has_erax)
     {
        WISEC(PUD_SECTION_ERAX, 2);
        W16(p->era, 1);
     }

   /* Section DIM */
   WISEC(PUD_SECTION_DIM, 4);
   W16(p->map_w, 1);
   W16(p->map_h, 1);

   /* Section UDTA */
   WISEC(PUD_SECTION_UDTA, 5696);
   WI16(p->private_data->default_udta, 1);
   for (i = 0; i < 110; i++) W16(p->units_descr[i].overlap_frames, 1);
   for (i = 0; i < 508; i++) W16(p->obsolete_udta[i], 1);
   for (i = 0; i < 110; i++) W32(p->units_descr[i].sight, 1);
   for (i = 0; i < 110; i++) W16(p->units_descr[i].hp, 1);
   for (i = 0; i < 110; i++) W8(p->units_descr[i].has_magic, 1);
   for (i = 0; i < 110; i++) W8(p->units_descr[i].build_time, 1);
   for (i = 0; i < 110; i++) W8(p->units_descr[i].gold_cost, 1);
   for (i = 0; i < 110; i++) W8(p->units_descr[i].lumber_cost, 1);
   for (i = 0; i < 110; i++) W8(p->units_descr[i].oil_cost, 1);
   // FIXME I think the words are switched!
   for (i = 0; i < 110; i++)
     {
        l = ((p->units_descr[i].size_w << 16) & 0xffff0000) | (p->units_descr[i].size_h & 0x0000ffff);
        W32(l, 1);
     }
   for (i = 0; i < 110; i++)
     {
        l = ((p->units_descr[i].box_w << 16) & 0xffff0000) | (p->units_descr[i].box_h & 0x0000ffff);
        W32(l, 1);
     }
   for (i = 0; i < 110; i++) W8(p->units_descr[i].range, 1);
   for (i = 0; i < 110; i++) W8(p->units_descr[i].computer_react_range, 1);
   for (i = 0; i < 110; i++) W8(p->units_descr[i].human_react_range, 1);
   for (i = 0; i < 110; i++) W8(p->units_descr[i].armor, 1);
   for (i = 0; i < 110; i++) WI8(p->units_descr[i].rect_sel, 1);
   for (i = 0; i < 110; i++) W8(p->units_descr[i].priority, 1);
   for (i = 0; i < 110; i++) W8(p->units_descr[i].basic_damage, 1);
   for (i = 0; i < 110; i++) W8(p->units_descr[i].piercing_damage, 1);
   for (i = 0; i < 110; i++) WI8(p->units_descr[i].weapons_upgradable, 1);
   for (i = 0; i < 110; i++) WI8(p->units_descr[i].armor_upgradable, 1);
   for (i = 0; i < 110; i++) W8(p->units_descr[i].missile_weapon, 1);
   for (i = 0; i < 110; i++) W8(p->units_descr[i].type, 1);
   for (i = 0; i < 110; i++) W8(p->units_descr[i].decay_rate, 1);
   for (i = 0; i < 110; i++) W8(p->units_descr[i].annoy, 1);
   for (i = 0; i < 58; i++) W8(p->units_descr[i].mouse_right_btn, 1);
   for (i = 0; i < 110; i++) W16(p->units_descr[i].point_value, 1);
   for (i = 0; i < 110; i++) W8(p->units_descr[i].can_target, 1);
   for (i = 0; i < 110; i++) W32(p->units_descr[i].flags, 1);
   // WI16(0, 127); // Obsolete data (do not print!!)

   /* Section ALOW */
   if (p->private_data->default_allow == 0)
     {
        WISEC(PUD_SECTION_ALOW, 384);
        fwrite(&p->unit_alow, sizeof(uint32_t), 16, f);
        fwrite(&p->spell_start, sizeof(uint32_t), 16, f);
        fwrite(&p->spell_alow, sizeof(uint32_t), 16, f);
        fwrite(&p->spell_acq, sizeof(uint32_t), 16, f);
        fwrite(&p->up_alow, sizeof(uint32_t), 16, f);
        fwrite(&p->up_acq, sizeof(uint32_t), 16, f);
        PUD_CHECK_FERROR(f, PUD_FALSE);
     }

   /* Section UGRD */
   WISEC(PUD_SECTION_UGRD, 782);
   WI16(p->private_data->default_ugrd, 1);
   for (i = 0; i < 52; i++) W8(p->upgrades[i].time, 1);
   for (i = 0; i < 52; i++) W16(p->upgrades[i].gold, 1);
   for (i = 0; i < 52; i++) W16(p->upgrades[i].lumber, 1);
   for (i = 0; i < 52; i++) W16(p->upgrades[i].oil, 1);
   for (i = 0; i < 52; i++) W16(p->upgrades[i].icon, 1);
   for (i = 0; i < 52; i++) W16(p->upgrades[i].group, 1);
   for (i = 0; i < 52; i++) W32(p->upgrades[i].flags, 1);

   /* Section SIDE */
   WISEC(PUD_SECTION_SIDE, 16);
   fwrite(&p->side, sizeof(uint8_t), 16, f);
   PUD_CHECK_FERROR(f, PUD_FALSE);

   /* Section SGLD */
   WISEC(PUD_SECTION_SGLD, 32);
   fwrite(&p->sgld, sizeof(uint16_t), 16, f);
   PUD_CHECK_FERROR(f, PUD_FALSE);

   /* Section SLBR */
   WISEC(PUD_SECTION_SLBR, 32);
   fwrite(&p->slbr, sizeof(uint16_t), 16, f);
   PUD_CHECK_FERROR(f, PUD_FALSE);

   /* Section SOIL */
   WISEC(PUD_SECTION_SOIL, 32);
   fwrite(&p->soil, sizeof(uint16_t), 16, f);
   PUD_CHECK_FERROR(f, PUD_FALSE);

   /* Section AIPL */
   WISEC(PUD_SECTION_AIPL, 16);
   fwrite(&p->ai, sizeof(uint8_t), 16, f);
   PUD_CHECK_FERROR(f, PUD_FALSE);

   /* Section MTMX */
   WSEC(PUD_SECTION_MTXM, map_len);
   fwrite(p->tiles_map, sizeof(uint16_t), p->tiles, f);
   PUD_CHECK_FERROR(f, PUD_FALSE);

   /* Section SQM */
   WSEC(PUD_SECTION_SQM, map_len);
   fwrite(p->movement_map, sizeof(uint16_t), p->tiles, f);
   PUD_CHECK_FERROR(f, PUD_FALSE);

   /* Section OILM */
   WSEC(PUD_SECTION_OILM, p->tiles);
   WI8(0, p->tiles);

   /* Section REGM */
   WSEC(PUD_SECTION_REGM, map_len);
   fwrite(p->action_map, sizeof(uint16_t), p->tiles, f);
   PUD_CHECK_FERROR(f, PUD_FALSE);

   /* Section UNIT */
   WSEC(PUD_SECTION_UNIT, units_len);
   for (i = 0; i < p->units_count; ++i)
     {
        W16(p->units[i].x, 1);
        W16(p->units[i].y, 1);
        W8(p->units[i].type, 1);
        W8(p->units[i].owner, 1);
        W16(p->units[i].alter, 1);
     }

#undef WSTR
#undef WISEC
#undef WI32
#undef WI16
#undef WI8
#undef W32
#undef W16
#undef W8

   fclose(f);

   return PUD_TRUE;
}

PUDAPI void
pud_version_set(Pud      *pud,
                uint16_t  version)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_W, VOID);
   pud->version = version;
}

PUDAPI void
pud_description_set(Pud        *pud,
                    const char *descr)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_W, VOID);
   if (descr)
     {
        strncpy(pud->description, descr, 31);
        pud->description[31] = '\0';
     }
}

PUDAPI const char *
pud_description_get(const Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, NULL);
   return pud->description;
}

PUDAPI void
pud_tag_set(Pud      *pud,
            uint32_t  tag)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_W, VOID);
   pud->tag = tag;
}

PUDAPI Pud_Bool
pud_unit_add(Pud          *pud,
             unsigned int  x,
             unsigned int  y,
             Pud_Player    owner,
             Pud_Unit      unit,
             uint16_t      alter)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_W, PUD_FALSE);

   size_t size;
   unsigned int nb;
   void *ptr;

   /* Override alter value for unspecified cases */
   if ((unit != PUD_UNIT_GOLD_MINE) && (unit != PUD_UNIT_OIL_PATCH))
     {
        //        if ((owner == PUD_OWNER_PASSIVE_COMPUTER) ||
        //            (owner == PUD_OWNER_RESCUE_PASSIVE))
        //          alter = 0;
        //        else
        //          alter = 1;
     }

   const Pud_Unit_Info u = {
      .x     = x,
      .y     = y,
      .type  = unit,
      .owner = owner,
      .alter = alter
   };

   if ((x > pud->map_w - 1) || (y > pud->map_h - 1))
     DIE_RETURN(PUD_FALSE, "Invalid indexes [%i][%i]", x, y);

   /* TODO Optimise memory allocation because it is not great */
   nb = pud->units_count + 1;
   size = nb * sizeof(Pud_Unit_Info);
   ptr = realloc(pud->units, size);
   /* On failure, keep the units as is */
   if (ptr == NULL) DIE_RETURN(PUD_FALSE, "Failed to alloc memory");
   pud->units = ptr;
   memcpy(&(pud->units[pud->units_count]), &u, sizeof(Pud_Unit_Info));

   pud->units_count = nb;

   return PUD_TRUE;
}

PUDAPI Pud_Bool
pud_tile_set(Pud          *pud,
             unsigned int  x,
             unsigned int  y,
             uint16_t      tile)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_W, PUD_FALSE);

   if ((x >= pud->map_w) || (y >= pud->map_h))
     DIE_RETURN(PUD_FALSE, "Invalid indexes (x=%u,y=%u)", x, y);

   pud->tiles_map[(y * pud->map_w) + x] = tile;
   return PUD_TRUE;
}

PUDAPI uint16_t
pud_tile_get(const Pud    *pud,
             unsigned int  x,
             unsigned int  y)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, 0x0000);

   if ((x >= pud->map_w) || (y >= pud->map_h))
     DIE_RETURN(0x0000, "Invalid indexes [%i][%i]", x, y);

   return pud->tiles_map[(y * pud->map_w) + x];
}

PUDAPI Pud_Error
pud_check(Pud                   *pud,
          Pud_Error_Description *err)
{
   unsigned int i;
   unsigned int starting_locations = 0;
   const Pud_Unit_Info *u;
   Pud_Error ret = PUD_ERROR_UNDEFINED;
   unsigned int players_units[16];
   Pud_Bool players_start_loc[16];

   memset(players_units, 0, sizeof(players_units));
   memset(players_start_loc, 0, sizeof(players_start_loc));

   if (!pud->private_data->init)
     {
        ret = PUD_ERROR_NOT_INITIALIZED;
        goto end;
     }

   for (i = 0; i < pud->units_count; ++i)
     {
        u = &(pud->units[i]);

        /* Check for any invalid owner (Pud_Player) */
        if (!((u->owner < 8) || (u->owner == PUD_PLAYER_NEUTRAL)))
          {
             ret = PUD_ERROR_INVALID_PLAYER;
             if (err) err->data.unit = u;
             goto end;
          }

        if ((u->type == PUD_UNIT_ORC_START) ||
            (u->type == PUD_UNIT_HUMAN_START))
          {
             ++starting_locations;

             /* Did we already register the start location for this player. */
             if (players_start_loc[u->owner] == PUD_TRUE)
               {
                  ret = PUD_ERROR_TOO_MUCH_START_LOCATIONS;
                  if (err) err->data.unit = u;
                  goto end;
               }
             players_start_loc[u->owner] = PUD_TRUE;
          }
        else
          {
             players_units[u->owner]++;
          }
     }

   if (starting_locations <= 1)
     {
        ret = PUD_ERROR_NOT_ENOUGH_START_LOCATIONS;
        if (err) err->data.count = starting_locations;
        goto end;
     }

   for (i = 0; i < 16; i++)
     {
        /* Player has units but no start location */
        if ((players_units[i] != 0) && (players_start_loc[i] == 0) &&
            (i != PUD_PLAYER_NEUTRAL))
          {
             ret = PUD_ERROR_NO_START_LOCATION;
             if (err) err->data.player = i;
             goto end;
          }

        /* There is one start location but no units */
        if ((players_units[i] == 0) && (players_start_loc[i] == PUD_TRUE))
          {
             ret = PUD_ERROR_EMPTY_PLAYER;
             if (err) err->data.player = i;
             goto end;
          }
     }

   /* If a player has no unit at all, it is controlled by nobody */
   for (i = 0; i < 8; i++)
     {
        if (players_units[i] == 0)
          {
             pud->owner.players[i] = PUD_OWNER_NOBODY;
          }
     }

   pud->starting_points = starting_locations;
   ret = PUD_ERROR_NONE;
end:
   if (err) err->type = ret;
   return ret;
}

PUDAPI Pud_Bool
pud_unit_building_is(Pud_Unit unit)
{
   return ((unit >= PUD_UNIT_FARM) && (unit <= PUD_UNIT_RUNESTONE) &&
           (unit != PUD_UNIT_HUMAN_START) && (unit != PUD_UNIT_ORC_START));
}

PUDAPI Pud_Bool
pud_unit_start_location_is(Pud_Unit unit)
{
   return ((unit == PUD_UNIT_HUMAN_START) || (unit == PUD_UNIT_ORC_START));
}

PUDAPI Pud_Bool
pud_unit_flying_is(Pud_Unit unit)
{
   return ((unit == PUD_UNIT_GNOMISH_FLYING_MACHINE) ||
           (unit == PUD_UNIT_GOBLIN_ZEPPLIN) ||
           (unit == PUD_UNIT_GRYPHON_RIDER) ||
           (unit == PUD_UNIT_DRAGON) ||
           (unit == PUD_UNIT_DEATHWING) ||
           (unit == PUD_UNIT_DAEMON) ||
           (unit == PUD_UNIT_KURDRAN_AND_SKY_REE));
}

PUDAPI Pud_Bool
pud_unit_land_is(Pud_Unit unit)
{
   return ((!pud_unit_underwater_is(unit)) &&
           (!pud_unit_boat_is(unit)) &&
           (!pud_unit_flying_is(unit)));
}

PUDAPI Pud_Bool
pud_unit_underwater_is(Pud_Unit unit)
{
   return ((unit == PUD_UNIT_GNOMISH_SUBMARINE) ||
           (unit == PUD_UNIT_GIANT_TURTLE));
}

PUDAPI Pud_Bool
pud_unit_marine_is(Pud_Unit unit)
{
   return ((pud_unit_underwater_is(unit)) ||
           (pud_unit_boat_is(unit)) ||
           (pud_unit_oil_well_is(unit)));
}

PUDAPI Pud_Bool
pud_unit_resource_collector_is(Pud_Unit unit)
{
   /*
    * WARNING: This function does not study gold
    * collectors because this check is already performed
    * by pud_unit_lumber_collector_is(), as gold collectors
    * are a subset of lumber collectors.
    */
   return (pud_unit_lumber_collector_is(unit) ||
           pud_unit_oil_collector_is(unit));
}

PUDAPI Pud_Bool
pud_unit_gold_collector_is(Pud_Unit unit)
{
   return ((unit == PUD_UNIT_GREAT_HALL) ||
           (unit == PUD_UNIT_TOWN_HALL) ||
           (unit == PUD_UNIT_STRONGHOLD) ||
           (unit == PUD_UNIT_KEEP) ||
           (unit == PUD_UNIT_CASTLE) ||
           (unit == PUD_UNIT_FORTRESS));
}

PUDAPI Pud_Bool
pud_unit_oil_collector_is(Pud_Unit unit)
{
   return ((unit == PUD_UNIT_HUMAN_SHIPYARD) ||
           (unit == PUD_UNIT_ORC_SHIPYARD) ||
           (unit == PUD_UNIT_HUMAN_REFINERY) ||
           (unit == PUD_UNIT_ORC_REFINERY));
}

PUDAPI Pud_Bool
pud_unit_lumber_collector_is(Pud_Unit unit)
{
   /* We check for gold collectors because a gold collector
    * can also collect lumber */
   return (pud_unit_gold_collector_is(unit) ||
           (unit == PUD_UNIT_TROLL_LUMBER_MILL) ||
           (unit == PUD_UNIT_ELVEN_LUMBER_MILL));
}

PUDAPI Pud_Bool
pud_unit_boat_is(Pud_Unit unit)
{
   return ((unit >= PUD_UNIT_HUMAN_TANKER) &&
           (unit <= PUD_UNIT_JUGGERNAUGHT));
}

PUDAPI Pud_Bool
pud_unit_coast_building_is(Pud_Unit unit)
{
   return ((unit == PUD_UNIT_ORC_SHIPYARD) ||
           (unit == PUD_UNIT_HUMAN_SHIPYARD) ||
           (unit == PUD_UNIT_ORC_FOUNDRY) ||
           (unit == PUD_UNIT_HUMAN_FOUNDRY) ||
           (unit == PUD_UNIT_HUMAN_REFINERY) ||
           (unit == PUD_UNIT_ORC_REFINERY));
}

PUDAPI Pud_Bool
pud_unit_always_passive_is(Pud_Unit unit)
{
   return (unit == PUD_UNIT_CRITTER);
}

PUDAPI Pud_Bool
pud_unit_oil_well_is(Pud_Unit unit)
{
   return ((unit == PUD_UNIT_OIL_PATCH) ||
           (unit == PUD_UNIT_HUMAN_OIL_WELL) ||
           (unit == PUD_UNIT_ORC_OIL_WELL));
}

PUDAPI Pud_Color
pud_minimap_color_for_player(Pud_Player player)
{
   switch (player)
     {
      case PUD_PLAYER_RED:    return color_make(0xc0, 0x00, 0x00, 0xff); // Red
      case PUD_PLAYER_BLUE:   return color_make(0x00, 0x00, 0xc0, 0xff); // Blue
      case PUD_PLAYER_GREEN:  return color_make(0x00, 0xff, 0x00, 0xff); // Green
      case PUD_PLAYER_VIOLET: return color_make(0x80, 0x00, 0xc0, 0xff); // Violet
      case PUD_PLAYER_ORANGE: return color_make(0xff, 0x80, 0x00, 0xff); // Orange
      case PUD_PLAYER_BLACK:  return color_make(0x00, 0x00, 0x00, 0xff); // Black
      case PUD_PLAYER_WHITE:  return color_make(0xff, 0xff, 0xff, 0xff); // White
      case PUD_PLAYER_YELLOW: return color_make(0xff, 0xd0, 0x00, 0xff); // Yellow
      case PUD_PLAYER_NEUTRAL:return color_make(0xa2, 0xa2, 0xa6, 0xff); // Neutral
      default: ERR("Invalid player %i", player); break;
     }

   return color_make(0xff, 0x00, 0xff, 0xff);
}

static inline Pud_Color
pud_gold_mine_color_get(void)
{
   return color_make(0xff, 0xff, 0x00, 0xff);
}

static inline Pud_Color
pud_oil_patch_color_get(void)
{
   return color_make(0x00, 0x00, 0x00, 0xff);
}

PUDAPI Pud_Color
pud_minimap_color_for_unit(Pud_Unit   unit,
                           Pud_Player player)
{
   if (unit == PUD_UNIT_GOLD_MINE)
     return pud_gold_mine_color_get();
   else if (unit == PUD_UNIT_OIL_PATCH)
     return pud_oil_patch_color_get();
   else
     return pud_minimap_color_for_player(player);
}

PUDAPI Pud_Side
pud_unit_side_get(Pud_Unit unit)
{
   switch (unit)
     {
      case PUD_UNIT_SKELETON:
      case PUD_UNIT_DAEMON:
      case PUD_UNIT_CRITTER:
      case PUD_UNIT_GOLD_MINE:
      case PUD_UNIT_OIL_PATCH:
         return PUD_SIDE_NEUTRAL;

      default:
         break;
     }

   if ((unit >= PUD_UNIT_CIRCLE_OF_POWER) && (unit <= PUD_UNIT_NONE))
     return PUD_SIDE_NEUTRAL;

   return (unit % 2 == 0) ? PUD_SIDE_HUMAN : PUD_SIDE_ORC;
}

PUDAPI Pud_Bool
pud_unit_valid_is(Pud_Unit unit)
{
   const unsigned u = (unsigned)unit;
   return ((u <= 0x6c) &&
           ((u != 0x22) && (u != 0x24) && (u != 0x25) &&
            (u != 0x30) && (u != 0x36)));
}

PUDAPI Pud_Unit
pud_unit_switch_side(Pud_Unit unit)
{
   if (pud_unit_hero_is(unit) ||
       (unit == PUD_UNIT_GOLD_MINE) ||
       (unit == PUD_UNIT_OIL_PATCH) ||
       ((unsigned)unit >= PUD_UNIT_CIRCLE_OF_POWER))
     {
        return unit;
     }

   if (unit % 2 == 0) return unit + 1; /* Human to Orc */
   else return unit - 1; /* Orc to Human */
}

PUDAPI Pud_Side
pud_side_for_player_get(const Pud *pud,
                        Pud_Player player)
{
   return (player == PUD_PLAYER_NEUTRAL)
      ? PUD_SIDE_NEUTRAL
      : pud->side.players[player];
}

PUDAPI Pud_Bool
pud_default_alow_get(const Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, PUD_FALSE);
   return pud->private_data->default_allow;
}

PUDAPI Pud_Bool
pud_default_udta_get(const Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, PUD_FALSE);
   return pud->private_data->default_udta;
}

PUDAPI Pud_Bool
pud_default_ugrd_get(const Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, PUD_FALSE);
   return pud->private_data->default_ugrd;
}

PUDAPI void
pud_default_alow_override(Pud      *pud,
                          Pud_Bool  use_default)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_W, VOID);
   pud->private_data->default_allow = !!use_default;
}

PUDAPI void
pud_default_ugrd_override(Pud      *pud,
                          Pud_Bool  use_default)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_W, VOID);
   pud->private_data->default_ugrd = !!use_default;
}

PUDAPI void
pud_default_udta_override(Pud      *pud,
                          Pud_Bool  use_default)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_W, VOID);
   pud->private_data->default_udta = !!use_default;
}
