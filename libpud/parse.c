/*
 * parse.c
 * libpud
 *
 * Copyright (c) 2014 Jean Guyomarc'h
 */

#include "pud_private.h"

/*
 * Parsing of individual sections is here
 */

#define HAS_SECTION(sec) pud->sections |= (1 << sec)

Pud_Bool
pud_parse_type(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, 0);

   char buf[16];
   uint32_t l;
   uint32_t chk;
   const char type[10] = {
      'W', 'A', 'R', '2', ' ', 'M', 'A', 'P', '\0', '\0'
   };

   chk = pud_go_to_section(pud, PUD_SECTION_TYPE);
   if (!chk) DIE_RETURN(PUD_FALSE, "Failed to reach section TYPE");
   PUD_VERBOSE(pud, 2, "At section TYPE (size = %u)", chk);
   HAS_SECTION(PUD_SECTION_TYPE);

   /* Read 10bytes + 2 unused */
   READBUF(pud, buf, uint8_t, 12, FAIL(PUD_FALSE));
   if (memcmp(buf, type, 10))
     DIE_RETURN(PUD_FALSE, "TYPE section has a wrong header (not a WAR2 MAP)");

   /* Read ID TAG */
   l = READ32(pud, FAIL(PUD_FALSE));

   pud->tag = l;

   return PUD_TRUE;
}

Pud_Bool
pud_parse_ver(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, PUD_FALSE);

   uint16_t w;
   uint32_t chk;

   chk = pud_go_to_section(pud, PUD_SECTION_VER);
   if (!chk) DIE_RETURN(PUD_FALSE, "Failed to reach section VER");
   PUD_VERBOSE(pud, 2, "At section VER (size = %u)", chk);
   HAS_SECTION(PUD_SECTION_VER);

   w = READ16(pud, FAIL(PUD_FALSE));

   pud->version = w;
   switch (w)
     {
      case 0x0011:
         pud->extension_pack = 0;
         break;

      case 0x013:
         pud->extension_pack = 1;
         break;

      default:
         fprintf(stderr, "*** Unknown version 0x%04x."
                 " Defaults to extension pack\n", w);
         pud->extension_pack = 1;
         break;
     }

   return PUD_TRUE;
}

Pud_Bool
pud_parse_desc(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, PUD_FALSE);

   uint32_t chk;
   char buf[32];

   chk = pud_go_to_section(pud, PUD_SECTION_DESC);
   if (!chk) DIE_RETURN(PUD_FALSE, "Failed to reach section DESC");
   PUD_VERBOSE(pud, 2, "At section DESC (size = %u)", chk);
   HAS_SECTION(PUD_SECTION_DESC);

   READBUF(pud, buf, char, 32, FAIL(PUD_FALSE));
   memcpy(pud->description, buf, 32 * sizeof(char));

   return PUD_TRUE;
}

Pud_Bool
pud_parse_ownr(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, PUD_FALSE);

   uint8_t buf[8];
   uint32_t len;

   len = pud_go_to_section(pud, PUD_SECTION_OWNR);
   if (!len) DIE_RETURN(PUD_FALSE, "Failed to reach section OWNR");
   PUD_VERBOSE(pud, 2, "At section OWNR (size = %u)", len);
   HAS_SECTION(PUD_SECTION_OWNR);

   READBUF(pud, buf, uint8_t, 8, FAIL(PUD_FALSE));
   memcpy(pud->owner.players, buf, 8 * sizeof(uint8_t));

   READBUF(pud, buf, uint8_t, 7, FAIL(PUD_FALSE));
   memcpy(pud->owner.unusable, buf, 7 * sizeof(uint8_t));

   READBUF(pud, buf, uint8_t, 1, FAIL(PUD_FALSE));
   memcpy(&(pud->owner.neutral), buf, 1 * sizeof(uint8_t));

   return PUD_TRUE;
}

Pud_Bool
pud_parse_side(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, PUD_FALSE);

   uint8_t buf[8];
   uint32_t len;

   len = pud_go_to_section(pud, PUD_SECTION_SIDE);
   if (!len) DIE_RETURN(PUD_FALSE, "Failed to reach section SIDE");
   PUD_VERBOSE(pud, 2, "At section SIDE (size = %u)", len);
   HAS_SECTION(PUD_SECTION_SIDE);

   READBUF(pud, buf, uint8_t, 8, FAIL(PUD_FALSE));
   memcpy(pud->side.players, buf, 8 * sizeof(uint8_t));

   READBUF(pud, buf, uint8_t, 7, FAIL(PUD_FALSE));
   memcpy(pud->side.unusable, buf, 7 * sizeof(uint8_t));

   READBUF(pud, buf, uint8_t, 1, FAIL(PUD_FALSE));
   memcpy(&(pud->side.neutral), buf, 1 * sizeof(uint8_t));

   return PUD_TRUE;
}

Pud_Bool
pud_parse_era(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, PUD_FALSE);

   uint32_t chk;
   uint16_t w;

   chk = pud_go_to_section(pud, PUD_SECTION_ERAX);
   if (!chk) // Optional section, use ERA by default
     {
        pud->has_erax = PUD_FALSE;
        PUD_VERBOSE(pud, 2, "Failed to find ERAX. Trying with ERA...");
        chk = pud_go_to_section(pud, PUD_SECTION_ERA);
        if (!chk) DIE_RETURN(PUD_FALSE, "Failed to reach section ERA");
        PUD_VERBOSE(pud, 2, "At section ERA (size = %u)", chk);
        HAS_SECTION(PUD_SECTION_ERA);
     }
   else
     {
        pud->has_erax = PUD_TRUE;
        PUD_VERBOSE(pud, 2, "At section ERAX (size = %u)", chk);
        HAS_SECTION(PUD_SECTION_ERAX);
     }

   w = READ16(pud, FAIL(PUD_FALSE));
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
         DIE_RETURN(PUD_FALSE, "Failed to parse Era [0x%x]", w);
         break;
     }

   return PUD_TRUE;
}

Pud_Bool
pud_parse_dim(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, PUD_FALSE);

   uint32_t chk;
   uint16_t x, y;
   Pud_Dimensions dim;
   Pud_Open_Mode mode;

   chk = pud_go_to_section(pud, PUD_SECTION_DIM);
   if (!chk) DIE_RETURN(PUD_FALSE, "Failed to reach section DIM");
   PUD_VERBOSE(pud, 2, "At section DIM (size = %u)", chk);
   HAS_SECTION(PUD_SECTION_DIM);

   x = READ16(pud, FAIL(PUD_FALSE));
   y = READ16(pud, FAIL(PUD_FALSE));

   if ((x == 32) && (y == 32))
     dim = PUD_DIMENSIONS_32_32;
   else if ((x == 64) && (y == 64))
     dim = PUD_DIMENSIONS_64_64;
   else if ((x == 96) && (y == 96))
     dim = PUD_DIMENSIONS_96_96;
   else if ((x == 128) && (y == 128))
     dim = PUD_DIMENSIONS_128_128;
   else
     DIE_RETURN(PUD_FALSE, "Invalid dimensions %i x %i", x, y);

   /* Override permissions because pud_dimensions_set() is
    * damn convenient to use */
   mode = pud->open_mode;
   pud->open_mode = PUD_OPEN_MODE_W;
   pud_dimensions_set(pud, dim);
   pud->open_mode = mode;

   return PUD_TRUE;
}

Pud_Bool
pud_parse_udta(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, PUD_FALSE);

   uint32_t chk;
   uint16_t wb[512];
   uint32_t lb[128];
   uint8_t bb[128];
   int i;

   chk = pud_go_to_section(pud, PUD_SECTION_UDTA);
   if (!chk) DIE_RETURN(PUD_FALSE, "Failed to reach section UDTA");
   PUD_VERBOSE(pud, 2, "At section UDTA (size = %u)", chk);
   HAS_SECTION(PUD_SECTION_UDTA);

   /* Use default data */
   READBUF(pud, wb, uint16_t, 1, FAIL(PUD_FALSE));
   pud->default_udta = !!wb[0];

   /* Overlap frames */
   READBUF(pud, wb, uint16_t, 110, FAIL(PUD_FALSE));
   for (i = 0; i < 110; i++)
     pud->unit_data[i].overlap_frames = wb[i];

   /* Obsolete data */
   READBUF(pud, pud->unkwown, uint16_t, 508, FAIL(PUD_FALSE));

   /* Sight (why the hell is it on 32 bits!?) */
   READBUF(pud, lb, uint32_t, 110, FAIL(PUD_FALSE));
   for (i = 0; i < 110; i++)
     pud->unit_data[i].sight = lb[i];

   /* Hit points */
   READBUF(pud, wb, uint16_t, 110, FAIL(PUD_FALSE));
   for (i = 0; i < 110; i++)
     pud->unit_data[i].hp = wb[i];

   /* Magic */
   READBUF(pud, bb, uint8_t, 110, FAIL(PUD_FALSE));
   for (i = 0; i < 110; i++)
     pud->unit_data[i].has_magic = !!bb[i];

   /* Build time */
   READBUF(pud, bb, uint8_t, 110, FAIL(PUD_FALSE));
   for (i = 0; i < 110; i++)
     pud->unit_data[i].build_time = bb[i];

   /* Gold cost */
   READBUF(pud, bb, uint8_t, 110, FAIL(PUD_FALSE));
   for (i = 0; i < 110; i++)
     pud->unit_data[i].gold_cost = bb[i];

   /* Lumber cost */
   READBUF(pud, bb, uint8_t, 110, FAIL(PUD_FALSE));
   for (i = 0; i < 110; i++)
     pud->unit_data[i].lumber_cost = bb[i];

   /* Oil cost */
   READBUF(pud, bb, uint8_t, 110, FAIL(PUD_FALSE));
   for (i = 0; i < 110; i++)
     pud->unit_data[i].oil_cost = bb[i];

   /* Unit size */
   READBUF(pud, lb, uint32_t, 110, FAIL(PUD_FALSE));
   for (i = 0; i < 110; i++)
     {
        pud->unit_data[i].size_w = (lb[i] >> 16) & 0x0000ffff;
        pud->unit_data[i].size_h = lb[i] & 0x0000ffff;
     }

   /* Unit box */
   READBUF(pud, lb, uint32_t, 110, FAIL(PUD_FALSE));
   for (i = 0; i < 110; i++)
     {
        pud->unit_data[i].box_w = (lb[i] >> 16) & 0x0000ffff;
        pud->unit_data[i].box_h = lb[i] & 0x0000ffff;
     }

   /* Attack range */
   READBUF(pud, bb, uint8_t, 110, FAIL(PUD_FALSE));
   for (i = 0; i < 110; i++)
     pud->unit_data[i].range = bb[i];

   /* React range (computer) */
   READBUF(pud, bb, uint8_t, 110, FAIL(PUD_FALSE));
   for (i = 0; i < 110; i++)
     pud->unit_data[i].computer_react_range = bb[i];

   /* React range (human) */
   READBUF(pud, bb, uint8_t, 110, FAIL(PUD_FALSE));
   for (i = 0; i < 110; i++)
     pud->unit_data[i].human_react_range = bb[i];

   /* Armor */
   READBUF(pud, bb, uint8_t, 110, FAIL(PUD_FALSE));
   for (i = 0; i < 110; i++)
     pud->unit_data[i].armor = bb[i];

   /* Selectable via rectangle */
   READBUF(pud, bb, uint8_t, 110, FAIL(PUD_FALSE));
   for (i = 0; i < 110; i++)
     pud->unit_data[i].rect_sel = !!bb[i];

   /* Priority */
   READBUF(pud, bb, uint8_t, 110, FAIL(PUD_FALSE));
   for (i = 0; i < 110; i++)
     pud->unit_data[i].priority = bb[i];

   /* Basic damage */
   READBUF(pud, bb, uint8_t, 110, FAIL(PUD_FALSE));
   for (i = 0; i < 110; i++)
     pud->unit_data[i].basic_damage = bb[i];

   /* Piercing damage */
   READBUF(pud, bb, uint8_t, 110, FAIL(PUD_FALSE));
   for (i = 0; i < 110; i++)
     pud->unit_data[i].piercing_damage = bb[i];

   /* Weapons upgradable */
   READBUF(pud, bb, uint8_t, 110, FAIL(PUD_FALSE));
   for (i = 0; i < 110; i++)
     pud->unit_data[i].weapons_upgradable = !!bb[i];

   /* Armor upgradable */
   READBUF(pud, bb, uint8_t, 110, FAIL(PUD_FALSE));
   for (i = 0; i < 110; i++)
     pud->unit_data[i].armor_upgradable = !!bb[i];

   /* Missile weapon */
   READBUF(pud, bb, uint8_t, 110, FAIL(PUD_FALSE));
   for (i = 0; i < 110; i++)
     pud->unit_data[i].missile_weapon = bb[i];

   /* Unit type */
   READBUF(pud, bb, uint8_t, 110, FAIL(PUD_FALSE));
   for (i = 0; i < 110; i++)
     pud->unit_data[i].type = bb[i];

   /* Decay rate */
   READBUF(pud, bb, uint8_t, 110, FAIL(PUD_FALSE));
   for (i = 0; i < 110; i++)
     pud->unit_data[i].decay_rate = bb[i];

   /* Annoy computer factor */
   READBUF(pud, bb, uint8_t, 110, FAIL(PUD_FALSE));
   for (i = 0; i < 110; i++)
     pud->unit_data[i].annoy = bb[i];

   /* 2nd mouse button action */
   READBUF(pud, bb, uint8_t, 58, FAIL(PUD_FALSE));
   for (i = 0; i < 58; i++)
     pud->unit_data[i].mouse_right_btn = bb[i];
   for (; i < 110; i++)
     pud->unit_data[i].mouse_right_btn = 0xff;

   /* Point value for killing unit */
   READBUF(pud, wb, uint16_t, 110, FAIL(PUD_FALSE));
   for (i = 0; i < 110; i++)
     pud->unit_data[i].point_value = wb[i];

   /* Can target */
   READBUF(pud, bb, uint8_t, 110, FAIL(PUD_FALSE));
   for (i = 0; i < 110; i++)
     pud->unit_data[i].can_target = bb[i];

   /* Flags */
   READBUF(pud, lb, uint32_t, 110, FAIL(PUD_FALSE));
   for (i = 0; i < 110; i++)
     pud->unit_data[i].flags = lb[i];

   /* Obsolete */
   if (chk == 5950)
     {
        READBUF(pud, lb, uint16_t, 127, FAIL(PUD_FALSE));
        PUD_VERBOSE(pud, 1, "Obsolete section in UDTA found. Skipping...");
     }

   return PUD_TRUE;
}

Pud_Bool
pud_parse_alow(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, PUD_FALSE);

   uint32_t chk;
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
        return PUD_TRUE;
     }
   PUD_VERBOSE(pud, 2, "At section ALOW (size = %u)", chk);
   HAS_SECTION(PUD_SECTION_ALOW);

   for (i = 0; i < ptrs_count; i++)
     {
        READBUF(pud, buf, uint32_t, 16, FAIL(PUD_FALSE));

        memcpy(&(ptrs[i]->players[0]),  &(buf[0]),  sizeof(uint32_t) * 8);
        memcpy(&(ptrs[i]->unusable[0]), &(buf[8]),  sizeof(uint32_t) * 7);
        memcpy(&(ptrs[i]->neutral),     &(buf[15]), sizeof(uint32_t) * 1);
     }

   return PUD_TRUE;
}

Pud_Bool
pud_parse_ugrd(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, PUD_FALSE);

   uint32_t chk;
   int i;
   uint8_t bb[64];
   uint16_t wb[64];
   uint32_t lb[64];

   chk = pud_go_to_section(pud, PUD_SECTION_UGRD);
   if (!chk) DIE_RETURN(PUD_FALSE, "Failed to reach section UGRD");
   PUD_VERBOSE(pud, 2, "At section UGRD (size = %u)", chk);
   HAS_SECTION(PUD_SECTION_UGRD);

   /* Use default data */
   READBUF(pud, wb, uint16_t, 1, FAIL(PUD_FALSE));
   pud->default_ugrd = !!wb[0];

   /* Upgrade time */
   READBUF(pud, bb, uint8_t, 52, FAIL(PUD_FALSE));
   for (i = 0; i < 52; i++)
     pud->upgrade[i].time = bb[i];

   /* Gold cost */
   READBUF(pud, wb, uint16_t, 52, FAIL(PUD_FALSE));
   for (i = 0; i < 52; i++)
     pud->upgrade[i].gold = wb[i];

   /* Lumber cost */
   READBUF(pud, wb, uint16_t, 52, FAIL(PUD_FALSE));
   for (i = 0; i < 52; i++)
     pud->upgrade[i].lumber = wb[i];

   /* Oil cost */
   READBUF(pud, wb, uint16_t, 52, FAIL(PUD_FALSE));
   for (i = 0; i < 52; i++)
     pud->upgrade[i].oil = wb[i];

   /* Icon */
   READBUF(pud, wb, uint16_t, 52, FAIL(PUD_FALSE));
   for (i = 0; i < 52; i++)
     pud->upgrade[i].icon = wb[i];

   /* Group */
   READBUF(pud, wb, uint16_t, 52, FAIL(PUD_FALSE));
   for (i = 0; i < 52; i++)
     pud->upgrade[i].group = wb[i];

   /* Flags */
   READBUF(pud, lb, uint32_t, 52, FAIL(PUD_FALSE));
   for (i = 0; i < 52; i++)
     pud->upgrade[i].flags = lb[i];

   return PUD_TRUE;
}

Pud_Bool
pud_parse_sgld(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, PUD_FALSE);

   uint32_t chk;
   uint16_t buf[16];

   chk = pud_go_to_section(pud, PUD_SECTION_SGLD);
   if (!chk) DIE_RETURN(PUD_FALSE, "Failed to reach section SGLD");
   PUD_VERBOSE(pud, 2, "At section SGLD (size = %u)", chk);
   HAS_SECTION(PUD_SECTION_SGLD);

   READBUF(pud, buf, uint16_t, 16, FAIL(PUD_FALSE));

   memcpy(&(pud->sgld.players[0]),  &(buf[0]),  sizeof(uint16_t) * 8);
   memcpy(&(pud->sgld.unusable[0]), &(buf[8]),  sizeof(uint16_t) * 7);
   memcpy(&(pud->sgld.neutral),     &(buf[15]), sizeof(uint16_t) * 1);

   return PUD_TRUE;
}

Pud_Bool
pud_parse_slbr(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, PUD_FALSE);

   uint32_t chk;
   uint16_t buf[16];

   chk = pud_go_to_section(pud, PUD_SECTION_SLBR);
   if (!chk) DIE_RETURN(PUD_FALSE, "Failed to reach section SLBR");
   PUD_VERBOSE(pud, 2, "At section SLBR (size = %u)", chk);
   HAS_SECTION(PUD_SECTION_SLBR);

   READBUF(pud, buf, uint16_t, 16, FAIL(PUD_FALSE));

   memcpy(&(pud->slbr.players[0]),  &(buf[0]),  sizeof(uint16_t) * 8);
   memcpy(&(pud->slbr.unusable[0]), &(buf[8]),  sizeof(uint16_t) * 7);
   memcpy(&(pud->slbr.neutral),     &(buf[15]), sizeof(uint16_t) * 1);

   return PUD_TRUE;
}

Pud_Bool
pud_parse_soil(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, PUD_FALSE);

   uint32_t chk;
   uint16_t buf[16];

   chk = pud_go_to_section(pud, PUD_SECTION_SOIL);
   if (!chk) DIE_RETURN(PUD_FALSE, "Failed to reach section SOIL");
   PUD_VERBOSE(pud, 2, "At section SOIL (size = %u)", chk);
   HAS_SECTION(PUD_SECTION_SOIL);

   READBUF(pud, buf, uint16_t, 16, FAIL(PUD_FALSE));

   memcpy(&(pud->soil.players[0]),  &(buf[0]),  sizeof(uint16_t) * 8);
   memcpy(&(pud->soil.unusable[0]), &(buf[8]),  sizeof(uint16_t) * 7);
   memcpy(&(pud->soil.neutral),     &(buf[15]), sizeof(uint16_t) * 1);

   return PUD_TRUE;
}

Pud_Bool
pud_parse_aipl(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, PUD_FALSE);

   uint32_t chk;
   uint8_t buf[16];

   chk = pud_go_to_section(pud, PUD_SECTION_AIPL);
   if (!chk) DIE_RETURN(PUD_FALSE, "Failed to reach section AIPL");
   PUD_VERBOSE(pud, 2, "At section AIPL (size = %u)", chk);
   HAS_SECTION(PUD_SECTION_AIPL);

   READBUF(pud, buf, uint8_t, 16, FAIL(PUD_FALSE));

   memcpy(&(pud->ai.players[0]),  &(buf[0]),  sizeof(uint8_t) * 8);
   memcpy(&(pud->ai.unusable[0]), &(buf[8]),  sizeof(uint8_t) * 7);
   memcpy(&(pud->ai.neutral),     &(buf[15]), sizeof(uint8_t) * 1);

   return PUD_TRUE;
}

Pud_Bool
pud_parse_mtxm(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, PUD_FALSE);

   uint32_t chk;
   uint16_t w;
   unsigned int i;

   chk = pud_go_to_section(pud, PUD_SECTION_MTXM);
   if (!chk) DIE_RETURN(PUD_FALSE, "Failed to reach section MTXM");
   PUD_VERBOSE(pud, 2, "At section MTXM (size = %u)", chk);
   HAS_SECTION(PUD_SECTION_MTXM);

   /* Check for integrity */
   if ((pud->tiles * sizeof(uint16_t)) != chk)
     DIE_RETURN(PUD_FALSE, "Mismatch between dims and tiles number");

   for (i = 0; i < pud->tiles; i++)
     {
        w = READ16(pud, FAIL(0));
        pud->tiles_map[i] = w;
     }

   return PUD_TRUE;
}

Pud_Bool
pud_parse_sqm(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, PUD_FALSE);

   uint32_t chk;
   uint16_t w;
   unsigned int i;

   chk = pud_go_to_section(pud, PUD_SECTION_SQM);
   if (!chk) DIE_RETURN(PUD_FALSE, "Failed to reach section SQM ");
   PUD_VERBOSE(pud, 2, "At section SQM  (size = %u)", chk);
   HAS_SECTION(PUD_SECTION_SQM);

   /* Check for integrity */
   if ((pud->tiles * sizeof(uint16_t)) != chk)
     DIE_RETURN(PUD_FALSE, "Mismatch between dims and tiles number");

   for (i = 0; i < pud->tiles; i++)
     {
        w = READ16(pud, FAIL(0));
        pud->movement_map[i] = w;
     }

   return PUD_TRUE;
}

Pud_Bool
pud_parse_oilm(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, PUD_FALSE);

   uint32_t chk;
   unsigned int i;
   uint8_t b;

   chk = pud_go_to_section(pud, PUD_SECTION_OILM);
   if (!chk) PUD_VERBOSE(pud, 2, "Section OILM (obsolete) not present. Skipping...");
   else
     {
        if (!pud->oil_map)
          pud->oil_map = malloc(pud->tiles * sizeof(uint8_t));
        for (i = 0; i < pud->tiles; i++)
          {
             b = READ8(pud, FAIL(0));
             pud->oil_map[i] = b;
          }
     }

   return PUD_TRUE;
}

Pud_Bool
pud_parse_regm(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, PUD_FALSE);

   uint32_t chk;
   uint16_t w;
   unsigned int i;

   chk = pud_go_to_section(pud, PUD_SECTION_REGM);
   if (!chk) DIE_RETURN(PUD_FALSE, "Failed to reach section REGM");
   PUD_VERBOSE(pud, 2, "At section REGM (size = %u)", chk);
   HAS_SECTION(PUD_SECTION_REGM);

   /* Check for integrity */
   if ((pud->tiles * sizeof(uint16_t)) != chk)
     DIE_RETURN(PUD_FALSE, "Mismatch between dims and tiles number");

   for (i = 0; i < pud->tiles; i++)
     {
        w = READ16(pud, FAIL(0));
        pud->action_map[i] = w;
     }

   return PUD_TRUE;
}

Pud_Bool
pud_parse_unit(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, PUD_FALSE);

   uint32_t chk;
   int units, size, i;

   chk = pud_go_to_section(pud, PUD_SECTION_UNIT);
   if (!chk) DIE_RETURN(PUD_FALSE, "Failed to reach section UNIT");
   PUD_VERBOSE(pud, 2, "At section UNIT (size = %u)", chk);
   HAS_SECTION(PUD_SECTION_UNIT);
   units = chk / 8;

   size = sizeof(Pud_Unit_Data) * units;
   pud->units = realloc(pud->units, size);
   if (!pud->units) DIE_RETURN(PUD_FALSE, "Failed to allocate memory");
   memset(pud->units, 0, size);
   pud->units_count = units;

   for (i = 0; i < units; ++i)
     {
        pud->units[i].x     = READ16(pud, ECHAP(err));
        pud->units[i].y     = READ16(pud, ECHAP(err));
        pud->units[i].type  = READ8(pud, ECHAP(err));
        pud->units[i].owner = READ8(pud, ECHAP(err));
        pud->units[i].alter = READ16(pud, ECHAP(err));
     }

   return PUD_TRUE;

err:
   free(pud->units);
   pud->units = NULL;
   return PUD_FALSE;
}
