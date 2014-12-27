#include "private.h"

/*
 * Parsing of individual sections is here
 */

bool
pud_parse_type(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, 0);

   char buf[16];
   FILE *f = pud->file;
   uint32_t l;
   uint32_t chk;

   chk = pud_go_to_section(pud, PUD_SECTION_TYPE);
   if (!chk) DIE_RETURN(false, "Failed to reach section TYPE");
   PUD_VERBOSE(pud, 2, "At section TYPE (size = %u)", chk);

   /* Read 10bytes + 2 unused */
   fread(buf, sizeof(uint8_t), 12, f);
   PUD_CHECK_FERROR(f, false);
   if (strncmp(buf, "WAR2 MAP\0\0", 10))
     DIE_RETURN(false, "TYPE section has a wrong header");

   /* Read ID TAG */
   fread(&l, sizeof(uint32_t), 1, f);
   PUD_CHECK_FERROR(f, false);

   pud->tag = l;

   return true;
}

bool
pud_parse_ver(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, false);

   FILE *f = pud->file;
   uint16_t w;
   uint32_t chk;

   chk = pud_go_to_section(pud, PUD_SECTION_VER);
   if (!chk) DIE_RETURN(false, "Failed to reach section VER");
   PUD_VERBOSE(pud, 2, "At section VER (size = %u)", chk);

   fread(&w, sizeof(uint16_t), 1, f);
   PUD_CHECK_FERROR(f, false);

   pud->version = w;
   return true;
}

bool
pud_parse_desc(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, false);

   uint32_t chk;
   FILE *f = pud->file;
   char buf[32];

   chk = pud_go_to_section(pud, PUD_SECTION_DESC);
   if (!chk) DIE_RETURN(false, "Failed to reach section DESC");
   PUD_VERBOSE(pud, 2, "At section DESC (size = %u)", chk);

   fread(buf, sizeof(char), 32, f);
   PUD_CHECK_FERROR(f, false);
   memcpy(pud->description, buf, 32 * sizeof(char));

   return true;
}

bool
pud_parse_ownr(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, false);

   uint8_t buf[8];
   uint32_t len;
   FILE *f = pud->file;

   len = pud_go_to_section(pud, PUD_SECTION_OWNR);
   if (!len) DIE_RETURN(false, "Failed to reach section OWNR");
   PUD_VERBOSE(pud, 2, "At section OWNR (size = %u)", len);

   fread(buf, sizeof(uint8_t), 8, f);
   PUD_CHECK_FERROR(f, false);
   memcpy(pud->owner.players, buf, 8 * sizeof(uint8_t));

   fread(buf, sizeof(uint8_t), 7, f);
   PUD_CHECK_FERROR(f, false);
   memcpy(pud->owner.unusable, buf, 7 * sizeof(uint8_t));

   fread(buf, sizeof(uint8_t), 1, f);
   PUD_CHECK_FERROR(f, false);
   memcpy(&(pud->owner.neutral), buf, 1 * sizeof(uint8_t));

   return true;
}

bool
pud_parse_side(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, false);

   uint8_t buf[8];
   uint32_t len;
   FILE *f = pud->file;

   len = pud_go_to_section(pud, PUD_SECTION_SIDE);
   if (!len) DIE_RETURN(false, "Failed to reach section SIDE");
   PUD_VERBOSE(pud, 2, "At section SIDE (size = %u)", len);

   fread(buf, sizeof(uint8_t), 8, f);
   PUD_CHECK_FERROR(f, false);
   memcpy(pud->side.players, buf, 8 * sizeof(uint8_t));

   fread(buf, sizeof(uint8_t), 7, f);
   PUD_CHECK_FERROR(f, false);
   memcpy(pud->side.unusable, buf, 7 * sizeof(uint8_t));

   fread(buf, sizeof(uint8_t), 1, f);
   PUD_CHECK_FERROR(f, false);
   memcpy(&(pud->side.neutral), buf, 1 * sizeof(uint8_t));

   return true;
}

bool
pud_parse_era(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, false);

   uint32_t chk;
   FILE *f = pud->file;
   uint16_t w;

   chk = pud_go_to_section(pud, PUD_SECTION_ERAX);
   if (!chk) // Optional section, use ERA by default
     {
        PUD_VERBOSE(pud, 2, "Failed to find ERAX. Trying with ERA...");
        chk = pud_go_to_section(pud, PUD_SECTION_ERA);
        if (!chk) DIE_RETURN(false, "Failed to reach section ERA");
        PUD_VERBOSE(pud, 2, "At section ERA (size = %u)", chk);
     }

   fread(&w, sizeof(uint16_t), 1, f);
   PUD_CHECK_FERROR(f, false);

   switch (w)
     {
      case 0x00:
      case 0x04 ... 0xff:
         pud->era = PUD_ERA_FOREST;
         break;

      case 0x01:
         pud->era = PUD_ERA_WINTER;
         break;

      case 0x02:
         pud->era = PUD_ERA_WASTELAND;
         break;

      case 0x03:
         pud->era = PUD_ERA_SWAMP;
         break;

      default:
         DIE_RETURN(false, "Failed to parse Era [%x]", w);
         break;
     }

   return true;
}

bool
pud_parse_dim(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, false);

   uint32_t chk;
   FILE *f = pud->file;
   uint16_t x, y;

   chk = pud_go_to_section(pud, PUD_SECTION_DIM);
   if (!chk) DIE_RETURN(false, "Failed to reach section DIM");
   PUD_VERBOSE(pud, 2, "At section DIM (size = %u)", chk);

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
     DIE_RETURN(false, "Invalid dimensions %i x %i", x, y);

   pud_dimensions_to_size(pud->dims, &pud->map_w, &pud->map_h);
   pud->tiles = pud->map_w * pud->map_h;

   return true;
}

bool
pud_parse_udta(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, false);

   uint32_t chk;
   FILE *f = pud->file;
   uint16_t wb[512];
   uint32_t lb[128];
   uint8_t bb[128];
   int i;

   chk = pud_go_to_section(pud, PUD_SECTION_UDTA);
   if (!chk) DIE_RETURN(false, "Failed to reach section UDTA");
   PUD_VERBOSE(pud, 2, "At section UDTA (size = %u)", chk);

   /* Use default data */
   fread(wb, sizeof(uint16_t), 1, f);
   PUD_CHECK_FERROR(f, false);
   pud->default_udta = !!wb[0];

   /* Overlap frames */
   fread(wb, sizeof(uint16_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].overlap_frames = wb[i];

   /* Obsolete data */
   fread(wb, sizeof(uint16_t), 508, f);
   PUD_CHECK_FERROR(f, false);

   /* Sight (why the hell is it on 32 bits!?) */
   fread(lb, sizeof(uint32_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].sight = lb[i];

   /* Hit points */
   fread(wb, sizeof(uint16_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].hp = wb[i];

   /* Magic */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].has_magic = !!bb[i];

   /* Build time */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].build_time = bb[i];

   /* Gold cost */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].gold_cost = bb[i];

   /* Lumber cost */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].lumber_cost = bb[i];

   /* Oil cost */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].oil_cost = bb[i];

   /* Unit size */
   fread(lb, sizeof(uint32_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     {
        pud->unit_data[i].size_w = (lb[i] >> 16) & 0x0000ffff;
        pud->unit_data[i].size_h = lb[i] & 0x0000ffff;
     }

   /* Unit box */
   fread(lb, sizeof(uint32_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     {
        pud->unit_data[i].box_w = (lb[i] >> 16) & 0x0000ffff;
        pud->unit_data[i].box_h = lb[i] & 0x0000ffff;
     }

   /* Attack range */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].range = bb[i];

   /* React range (computer) */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].computer_react_range = bb[i];

   /* React range (human) */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].human_react_range = bb[i];

   /* Armor */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].armor = bb[i];

   /* Selectable via rectangle */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].rect_sel = !!bb[i];

   /* Priority */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].priority = bb[i];

   /* Basic damage */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].basic_damage = bb[i];

   /* Piercing damage */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].piercing_damage = bb[i];

   /* Weapons upgradable */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].weapons_upgradable = !!bb[i];

   /* Armor upgradable */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].armor_upgradable = !!bb[i];

   /* Missile weapon */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].missile_weapon = bb[i];

   /* Unit type */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].type = bb[i];

   /* Decay rate */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].decay_rate = bb[i];

   /* Annoy computer factor */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].annoy = bb[i];

   /* 2nd mouse button action */
   fread(bb, sizeof(uint8_t), 58, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 58; i++)
     pud->unit_data[i].mouse_right_btn = bb[i];
   for (; i < 110; i++)
     pud->unit_data[i].mouse_right_btn = 0xff;

   /* Point value for killing unit */
   fread(wb, sizeof(uint16_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].point_value = wb[i];

   /* Can target */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].can_target = bb[i];

   /* Flags */
   fread(lb, sizeof(uint32_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].flags = lb[i];

   /* Obsolete */
   fread(wb, sizeof(uint16_t), 127, f);
   PUD_CHECK_FERROR(f, false);

   return true;
}

bool
pud_parse_alow(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, false);

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
        PUD_VERBOSE(pud, 2, "Section ALOW (optional) not present. Skipping...");
        pud->default_allow = 1;
        return true;
     }
   PUD_VERBOSE(pud, 2, "At section ALOW (size = %u)", chk);

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
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, false);

   uint32_t chk;
   FILE *f = pud->file;
   int i;
   uint8_t bb[64];
   uint16_t wb[64];
   uint32_t lb[64];

   chk = pud_go_to_section(pud, PUD_SECTION_UGRD);
   if (!chk) DIE_RETURN(false, "Failed to reach section UGRD");
   PUD_VERBOSE(pud, 2, "At section UGRD (size = %u)", chk);

   /* Use default data */
   fread(wb, sizeof(uint16_t), 1, f);
   PUD_CHECK_FERROR(f, false);
   pud->default_ugrd = !!wb[0];

   /* Upgrade time */
   fread(bb, sizeof(uint8_t), 52, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 52; i++)
     pud->upgrade[i].time = bb[i];

   /* Gold cost */
   fread(wb, sizeof(uint16_t), 52, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 52; i++)
     pud->upgrade[i].gold = wb[i];

   /* Lumber cost */
   fread(wb, sizeof(uint16_t), 52, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 52; i++)
     pud->upgrade[i].lumber = wb[i];

   /* Oil cost */
   fread(wb, sizeof(uint16_t), 52, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 52; i++)
     pud->upgrade[i].oil = wb[i];

   /* Icon */
   fread(wb, sizeof(uint16_t), 52, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 52; i++)
     pud->upgrade[i].icon = wb[i];

   /* Group */
   fread(wb, sizeof(uint16_t), 52, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 52; i++)
     pud->upgrade[i].group = wb[i];

   /* Flags */
   fread(lb, sizeof(uint32_t), 52, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 52; i++)
     pud->upgrade[i].flags = lb[i];

   return true;
}

bool
pud_parse_sgld(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, false);

   uint32_t chk;
   FILE *f = pud->file;
   uint16_t buf[16];

   chk = pud_go_to_section(pud, PUD_SECTION_SGLD);
   if (!chk) DIE_RETURN(false, "Failed to reach section SGLD");
   PUD_VERBOSE(pud, 2, "At section SGLD (size = %u)", chk);

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
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, false);

   uint32_t chk;
   FILE *f = pud->file;
   uint16_t buf[16];

   chk = pud_go_to_section(pud, PUD_SECTION_SLBR);
   if (!chk) DIE_RETURN(false, "Failed to reach section SLBR");
   PUD_VERBOSE(pud, 2, "At section SLBR (size = %u)", chk);

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
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, false);

   uint32_t chk;
   FILE *f = pud->file;
   uint16_t buf[16];

   chk = pud_go_to_section(pud, PUD_SECTION_SOIL);
   if (!chk) DIE_RETURN(false, "Failed to reach section SOIL");
   PUD_VERBOSE(pud, 2, "At section SOIL (size = %u)", chk);

   fread(buf, sizeof(uint16_t), 16, f);
   PUD_CHECK_FERROR(f, false);

   memcpy(&(pud->soil.players[0]),  &(buf[0]),  sizeof(uint16_t) * 8);
   memcpy(&(pud->soil.unusable[0]), &(buf[8]),  sizeof(uint16_t) * 7);
   memcpy(&(pud->soil.neutral),     &(buf[15]), sizeof(uint16_t) * 1);

   return true;
}

bool
pud_parse_aipl(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, false);

   uint32_t chk;
   FILE *f = pud->file;
   uint8_t buf[16];

   chk = pud_go_to_section(pud, PUD_SECTION_AIPL);
   if (!chk) DIE_RETURN(false, "Failed to reach section AIPL");
   PUD_VERBOSE(pud, 2, "At section AIPL (size = %u)", chk);

   fread(buf, sizeof(uint8_t), 16, f);
   PUD_CHECK_FERROR(f, false);

   memcpy(&(pud->ai.players[0]),  &(buf[0]),  sizeof(uint8_t) * 8);
   memcpy(&(pud->ai.unusable[0]), &(buf[8]),  sizeof(uint8_t) * 7);
   memcpy(&(pud->ai.neutral),     &(buf[15]), sizeof(uint8_t) * 1);

   return true;
}

bool
pud_parse_mtxm(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, false);

   uint32_t chk;
   FILE *f = pud->file;
   uint16_t w;
   int i;

   chk = pud_go_to_section(pud, PUD_SECTION_MTXM);
   if (!chk) DIE_RETURN(false, "Failed to reach section MTXM");
   PUD_VERBOSE(pud, 2, "At section MTXM (size = %u)", chk);

   /* Check for integrity */
   if ((pud->tiles * sizeof(uint16_t)) != chk)
     DIE_RETURN(false, "Mismatch between dims and tiles number");

   /* realloc() to avoid memory leaks when parsing twice */
   pud->tiles_map = realloc(pud->tiles_map, chk);
   if (!pud->tiles_map) DIE_RETURN(false, "Failed to allocate memory");
   memset(pud->tiles_map, 0, chk);

   for (i = 0; i < pud->tiles; i++)
     {
        fread(&w, sizeof(uint16_t), 1, f);
        PUD_CHECK_FERROR(f, false);
        pud->tiles_map[i] = w;
     }

   return true;
}

bool
pud_parse_sqm(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, false);

   uint32_t chk;
   FILE *f = pud->file;
   uint16_t w;
   int i;

   chk = pud_go_to_section(pud, PUD_SECTION_SQM);
   if (!chk) DIE_RETURN(false, "Failed to reach section SQM ");
   PUD_VERBOSE(pud, 2, "At section SQM  (size = %u)", chk);

   /* Check for integrity */
   if ((pud->tiles * sizeof(uint16_t)) != chk)
     DIE_RETURN(false, "Mismatch between dims and tiles number");

   /* realloc() to avoid memory leaks when parsing twice */
   pud->movement_map = realloc(pud->movement_map, chk);
   if (!pud->movement_map) DIE_RETURN(false, "Failed to allocate memory");
   memset(pud->movement_map, 0, chk);

   for (i = 0; i < pud->tiles; i++)
     {
        fread(&w, sizeof(uint16_t), 1, f);
        PUD_CHECK_FERROR(f, false);
        pud->movement_map[i] = w;
     }

   return true;
}

bool
pud_parse_oilm(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, false);

   uint32_t chk;

   chk = pud_go_to_section(pud, PUD_SECTION_OILM);
   if (!chk) PUD_VERBOSE(pud, 2, "Section OILM (obsolete) not present. Skipping...");
   else PUD_VERBOSE(pud, 2, "Section OILM (obsolete) present. Skipping...");

   return true;
}

bool
pud_parse_regm(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, false);

   uint32_t chk;
   FILE *f = pud->file;
   uint16_t w;
   int i;

   chk = pud_go_to_section(pud, PUD_SECTION_REGM);
   if (!chk) DIE_RETURN(false, "Failed to reach section REGM");
   PUD_VERBOSE(pud, 2, "At section REGM (size = %u)", chk);

   /* Check for integrity */
   if ((pud->tiles * sizeof(uint16_t)) != chk)
     DIE_RETURN(false, "Mismatch between dims and tiles number");

   /* realloc() to avoid memory leaks when parsing twice */
   pud->action_map = realloc(pud->action_map, chk);
   if (!pud->action_map) DIE_RETURN(false, "Failed to allocate memory");
   memset(pud->action_map, 0, chk);

   for (i = 0; i < pud->tiles; i++)
     {
        fread(&w, sizeof(uint16_t), 1, f);
        PUD_CHECK_FERROR(f, false);
        pud->action_map[i] = w;
     }

   return true;
}

bool
pud_parse_unit(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, false);

   uint32_t chk;
   FILE *f = pud->file;
   int units;

   chk = pud_go_to_section(pud, PUD_SECTION_UNIT);
   if (!chk) DIE_RETURN(false, "Failed to reach section UNIT");
   PUD_VERBOSE(pud, 2, "At section UNIT (size = %u)", chk);
   units = chk / 8;

   if (pud->units) free(pud->units);
   pud->units = calloc(units, sizeof(struct _unit));
   if (!pud->units) DIE_RETURN(false, "Failed to allocate memory");
   pud->units_count = units;

   fread(pud->units, sizeof(struct _unit), units, f);
   PUD_CHECK_FERROR(f, false);

   return true;
}

