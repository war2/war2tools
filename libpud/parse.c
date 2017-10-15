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

#include "pud_private.h"

/*
 * Parsing of individual sections is here
 */

#define HAS_SECTION(sec) pud->private_data->sections |= (1 << sec)

PUDAPI Pud_Bool
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

   PUD_TRAP_SETUP(pud) { return PUD_FALSE; }
   /* Read 10bytes + 2 unused */
   PUD_READ_BUFFER(pud, buf, 12);
   if (memcmp(buf, type, 10))
     DIE_RETURN(PUD_FALSE, "TYPE section has a wrong header (not a WAR2 MAP)");

   /* Read ID TAG */
   l = PUD_READ32(pud);

   pud->tag = l;

   return PUD_TRUE;
}

PUDAPI Pud_Bool
pud_parse_ver(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, PUD_FALSE);

   uint16_t w;
   uint32_t chk;

   chk = pud_go_to_section(pud, PUD_SECTION_VER);
   if (!chk) DIE_RETURN(PUD_FALSE, "Failed to reach section VER");
   PUD_VERBOSE(pud, 2, "At section VER (size = %u)", chk);
   HAS_SECTION(PUD_SECTION_VER);

   PUD_TRAP_SETUP(pud) { return PUD_FALSE; }
   w = PUD_READ16(pud);

   pud->version = w;
   return PUD_TRUE;
}

PUDAPI Pud_Bool
pud_parse_desc(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, PUD_FALSE);

   uint32_t chk;

   chk = pud_go_to_section(pud, PUD_SECTION_DESC);
   if (!chk) DIE_RETURN(PUD_FALSE, "Failed to reach section DESC");
   PUD_VERBOSE(pud, 2, "At section DESC (size = %u)", chk);
   HAS_SECTION(PUD_SECTION_DESC);

   PUD_TRAP_SETUP(pud) { return PUD_FALSE; }
   PUD_READ_BUFFER(pud, pud->description, 32);

   return PUD_TRUE;
}

PUDAPI Pud_Bool
pud_parse_ownr(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, PUD_FALSE);

   uint32_t len;

   len = pud_go_to_section(pud, PUD_SECTION_OWNR);
   if (!len) DIE_RETURN(PUD_FALSE, "Failed to reach section OWNR");
   PUD_VERBOSE(pud, 2, "At section OWNR (size = %u)", len);
   HAS_SECTION(PUD_SECTION_OWNR);

   PUD_TRAP_SETUP(pud) { return PUD_FALSE; }
   PUD_READ_BUFFER(pud, pud->owner.players, 8);
   PUD_READ_BUFFER(pud, pud->owner.unusable, 7);
   PUD_READ_BUFFER(pud, &(pud->owner.neutral), 1);

   return PUD_TRUE;
}

PUDAPI Pud_Bool
pud_parse_side(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, PUD_FALSE);

   const uint32_t len = pud_go_to_section(pud, PUD_SECTION_SIDE);
   if (!len) DIE_RETURN(PUD_FALSE, "Failed to reach section SIDE");
   PUD_VERBOSE(pud, 2, "At section SIDE (size = %u)", len);
   HAS_SECTION(PUD_SECTION_SIDE);

   PUD_TRAP_SETUP(pud) { return PUD_FALSE; }
   PUD_READ_BUFFER(pud, pud->side.players, 8);
   PUD_READ_BUFFER(pud, pud->side.unusable, 7);
   PUD_READ_BUFFER(pud, &(pud->side.neutral), 1);

   return PUD_TRUE;
}

PUDAPI Pud_Bool
pud_parse_era(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, PUD_FALSE);

   uint32_t chk;
   uint16_t w;

   chk = pud_go_to_section(pud, PUD_SECTION_ERAX);
   if (!chk) // Optional section, use ERA by default
     {
        pud->private_data->has_erax = PUD_FALSE;
        PUD_VERBOSE(pud, 2, "Failed to find ERAX. Trying with ERA...");
        chk = pud_go_to_section(pud, PUD_SECTION_ERA);
        if (!chk) DIE_RETURN(PUD_FALSE, "Failed to reach section ERA");
        PUD_VERBOSE(pud, 2, "At section ERA (size = %u)", chk);
        HAS_SECTION(PUD_SECTION_ERA);
     }
   else
     {
        pud->private_data->has_erax = PUD_TRUE;
        PUD_VERBOSE(pud, 2, "At section ERAX (size = %u)", chk);
        HAS_SECTION(PUD_SECTION_ERAX);
     }

   PUD_TRAP_SETUP(pud) { return PUD_FALSE; }
   w = PUD_READ16(pud);

   if ((w == 0x00) || ((w >= 0x04) && (w <= 0xff)))
      pud->era = PUD_ERA_FOREST;
   else if (w == 0x01)
      pud->era = PUD_ERA_WINTER;
   else if (w == 0x02)
      pud->era = PUD_ERA_WASTELAND;
   else if (w == 0x03)
      pud->era = PUD_ERA_SWAMP;
   else
      DIE_RETURN(PUD_FALSE, "Failed to parse Era [0x%x]", w);

   return PUD_TRUE;
}

PUDAPI Pud_Bool
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

   PUD_TRAP_SETUP(pud) { return PUD_FALSE; }

   x = PUD_READ16(pud);
   y = PUD_READ16(pud);

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
   mode = pud->private_data->open_mode;
   pud->private_data->open_mode = PUD_OPEN_MODE_W;
   pud_dimensions_set(pud, dim);
   pud->private_data->open_mode = mode;

   return PUD_TRUE;
}

PUDAPI Pud_Bool
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

   PUD_TRAP_SETUP(pud) { return PUD_FALSE; }

   /* Use default data */
   pud->private_data->default_udta = !!PUD_READ16(pud);

   /* Overlap frames */
   PUD_READ_BUFFER(pud, wb, sizeof(uint16_t) * 110);
   for (i = 0; i < 110; i++)
     pud->units_descr[i].overlap_frames = wb[i];

   /* Obsolete data */
   PUD_READ_BUFFER(pud, pud->obsolete_udta, sizeof(uint16_t) * 508);

   /* Sight (why the hell is it on 32 bits!?) */
   PUD_READ_BUFFER(pud, lb, sizeof(uint32_t) * 110);
   for (i = 0; i < 110; i++)
     pud->units_descr[i].sight = lb[i];

   /* Hit points */
   PUD_READ_BUFFER(pud, wb, sizeof(uint16_t) * 110);
   for (i = 0; i < 110; i++)
     pud->units_descr[i].hp = wb[i];

   /* Magic */
   PUD_READ_BUFFER(pud, bb, sizeof(uint8_t) * 110);
   for (i = 0; i < 110; i++)
     pud->units_descr[i].has_magic = !!bb[i];

   /* Build time */
   PUD_READ_BUFFER(pud, bb, sizeof(uint8_t) * 110);
   for (i = 0; i < 110; i++)
     pud->units_descr[i].build_time = bb[i];

   /* Gold cost */
   PUD_READ_BUFFER(pud, bb, sizeof(uint8_t) * 110);
   for (i = 0; i < 110; i++)
     pud->units_descr[i].gold_cost = bb[i];

   /* Lumber cost */
   PUD_READ_BUFFER(pud, bb, sizeof(uint8_t) * 110);
   for (i = 0; i < 110; i++)
     pud->units_descr[i].lumber_cost = bb[i];

   /* Oil cost */
   PUD_READ_BUFFER(pud, bb, sizeof(uint8_t) * 110);
   for (i = 0; i < 110; i++)
     pud->units_descr[i].oil_cost = bb[i];

   /* Unit size */
   PUD_READ_BUFFER(pud, lb, sizeof(uint32_t) * 110);
   for (i = 0; i < 110; i++)
     {
        pud->units_descr[i].size_w = (lb[i] >> 16) & 0x0000ffff;
        pud->units_descr[i].size_h = lb[i] & 0x0000ffff;
     }

   /* Unit box */
   PUD_READ_BUFFER(pud, lb, sizeof(uint32_t) * 110);
   for (i = 0; i < 110; i++)
     {
        pud->units_descr[i].box_w = (lb[i] >> 16) & 0x0000ffff;
        pud->units_descr[i].box_h = lb[i] & 0x0000ffff;
     }

   /* Attack range */
   PUD_READ_BUFFER(pud, bb, sizeof(uint8_t) * 110);
   for (i = 0; i < 110; i++)
     pud->units_descr[i].range = bb[i];

   /* React range (computer) */
   PUD_READ_BUFFER(pud, bb, sizeof(uint8_t) * 110);
   for (i = 0; i < 110; i++)
     pud->units_descr[i].computer_react_range = bb[i];

   /* React range (human) */
   PUD_READ_BUFFER(pud, bb, sizeof(uint8_t) * 110);
   for (i = 0; i < 110; i++)
     pud->units_descr[i].human_react_range = bb[i];

   /* Armor */
   PUD_READ_BUFFER(pud, bb, sizeof(uint8_t) * 110);
   for (i = 0; i < 110; i++)
     pud->units_descr[i].armor = bb[i];

   /* Selectable via rectangle */
   PUD_READ_BUFFER(pud, bb, sizeof(uint8_t) * 110);
   for (i = 0; i < 110; i++)
     pud->units_descr[i].rect_sel = !!bb[i];

   /* Priority */
   PUD_READ_BUFFER(pud, bb, sizeof(uint8_t) * 110);
   for (i = 0; i < 110; i++)
     pud->units_descr[i].priority = bb[i];

   /* Basic damage */
   PUD_READ_BUFFER(pud, bb, sizeof(uint8_t) * 110);
   for (i = 0; i < 110; i++)
     pud->units_descr[i].basic_damage = bb[i];

   /* Piercing damage */
   PUD_READ_BUFFER(pud, bb, sizeof(uint8_t) * 110);
   for (i = 0; i < 110; i++)
     pud->units_descr[i].piercing_damage = bb[i];

   /* Weapons upgradable */
   PUD_READ_BUFFER(pud, bb, sizeof(uint8_t) * 110);
   for (i = 0; i < 110; i++)
     pud->units_descr[i].weapons_upgradable = !!bb[i];

   /* Armor upgradable */
   PUD_READ_BUFFER(pud, bb, sizeof(uint8_t) * 110);
   for (i = 0; i < 110; i++)
     pud->units_descr[i].armor_upgradable = !!bb[i];

   /* Missile weapon */
   PUD_READ_BUFFER(pud, bb, sizeof(uint8_t) * 110);
   for (i = 0; i < 110; i++)
     pud->units_descr[i].missile_weapon = bb[i];

   /* Unit type */
   PUD_READ_BUFFER(pud, bb, sizeof(uint8_t) * 110);
   for (i = 0; i < 110; i++)
     pud->units_descr[i].type = bb[i];

   /* Decay rate */
   PUD_READ_BUFFER(pud, bb, sizeof(uint8_t) * 110);
   for (i = 0; i < 110; i++)
     pud->units_descr[i].decay_rate = bb[i];

   /* Annoy computer factor */
   PUD_READ_BUFFER(pud, bb, sizeof(uint8_t) * 110);
   for (i = 0; i < 110; i++)
     pud->units_descr[i].annoy = bb[i];

   /* 2nd mouse button action */
   PUD_READ_BUFFER(pud, bb, sizeof(uint8_t) * 58);
   for (i = 0; i < 58; i++)
     pud->units_descr[i].mouse_right_btn = bb[i];
   for (; i < 110; i++)
     pud->units_descr[i].mouse_right_btn = 0xff;

   /* Point value for killing unit */
   PUD_READ_BUFFER(pud, wb, sizeof(uint16_t) * 110);
   for (i = 0; i < 110; i++)
     pud->units_descr[i].point_value = wb[i];

   /* Can target */
   PUD_READ_BUFFER(pud, bb, sizeof(uint8_t) * 110);
   for (i = 0; i < 110; i++)
     pud->units_descr[i].can_target = bb[i];

   /* Flags */
   PUD_READ_BUFFER(pud, lb, sizeof(uint32_t) * 110);
   for (i = 0; i < 110; i++)
     pud->units_descr[i].flags = lb[i];

   /* Obsolete */
   if (chk == 5950)
     {
        PUD_READ_BUFFER(pud, lb, sizeof(uint16_t) * 127);
        PUD_VERBOSE(pud, 1, "Obsolete section in UDTA found. Skipping...");
     }

   return PUD_TRUE;
}

PUDAPI Pud_Bool
pud_parse_alow(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, PUD_FALSE);

   uint32_t chk;
   uint32_t buf[16];
   struct allow *ptrs[] = {
      &(pud->unit_alow),
      &(pud->spell_start),
      &(pud->spell_alow),
      &(pud->spell_acq),
      &(pud->up_alow),
      &(pud->up_acq)
   };
   const int ptrs_count = sizeof(ptrs) / sizeof(void *);
   int i;

   pud->private_data->default_allow = 0; // Reset before checking
   chk = pud_go_to_section(pud, PUD_SECTION_ALOW);
   if (!chk)
     {
        PUD_VERBOSE(pud, 2, "Section ALOW (optional) not present. Skipping...");
        pud->private_data->default_allow = 1;
        return PUD_TRUE;
     }
   PUD_VERBOSE(pud, 2, "At section ALOW (size = %u)", chk);
   HAS_SECTION(PUD_SECTION_ALOW);

   PUD_TRAP_SETUP(pud) { return PUD_FALSE; }
   for (i = 0; i < ptrs_count; i++)
     {
        PUD_READ_BUFFER(pud, buf, sizeof(uint32_t) * 16);

        memcpy(&(ptrs[i]->players[0]),  &(buf[0]),  sizeof(uint32_t) * 8);
        memcpy(&(ptrs[i]->unusable[0]), &(buf[8]),  sizeof(uint32_t) * 7);
        memcpy(&(ptrs[i]->neutral),     &(buf[15]), sizeof(uint32_t) * 1);
     }

   return PUD_TRUE;
}

PUDAPI Pud_Bool
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

   PUD_TRAP_SETUP(pud) { return PUD_FALSE; }

   /* Use default data */
   PUD_READ_BUFFER(pud, wb, sizeof(uint16_t));
   pud->private_data->default_ugrd = !!wb[0];

   /* upgrades time */
   PUD_READ_BUFFER(pud, bb, sizeof(uint8_t) * 52);
   for (i = 0; i < 52; i++)
     pud->upgrades[i].time = bb[i];

   /* Gold cost */
   PUD_READ_BUFFER(pud, wb, sizeof(uint16_t) * 52);
   for (i = 0; i < 52; i++)
     pud->upgrades[i].gold = wb[i];

   /* Lumber cost */
   PUD_READ_BUFFER(pud, wb, sizeof(uint16_t) * 52);
   for (i = 0; i < 52; i++)
     pud->upgrades[i].lumber = wb[i];

   /* Oil cost */
   PUD_READ_BUFFER(pud, wb, sizeof(uint16_t) * 52);
   for (i = 0; i < 52; i++)
     pud->upgrades[i].oil = wb[i];

   /* Icon */
   PUD_READ_BUFFER(pud, wb, sizeof(uint16_t) * 52);
   for (i = 0; i < 52; i++)
     pud->upgrades[i].icon = wb[i];

   /* Group */
   PUD_READ_BUFFER(pud, wb, sizeof(uint16_t) * 52);
   for (i = 0; i < 52; i++)
     pud->upgrades[i].group = wb[i];

   /* Flags */
   PUD_READ_BUFFER(pud, lb, sizeof(uint32_t) * 52);
   for (i = 0; i < 52; i++)
     pud->upgrades[i].flags = lb[i];

   return PUD_TRUE;
}

PUDAPI Pud_Bool
pud_parse_sgld(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, PUD_FALSE);

   uint32_t chk;
   uint16_t buf[16];

   chk = pud_go_to_section(pud, PUD_SECTION_SGLD);
   if (!chk) DIE_RETURN(PUD_FALSE, "Failed to reach section SGLD");
   PUD_VERBOSE(pud, 2, "At section SGLD (size = %u)", chk);
   HAS_SECTION(PUD_SECTION_SGLD);

   PUD_TRAP_SETUP(pud) { return PUD_FALSE; }
   PUD_READ_BUFFER(pud, buf, sizeof(uint16_t) * 16);

   memcpy(&(pud->sgld.players[0]),  &(buf[0]),  sizeof(uint16_t) * 8);
   memcpy(&(pud->sgld.unusable[0]), &(buf[8]),  sizeof(uint16_t) * 7);
   memcpy(&(pud->sgld.neutral),     &(buf[15]), sizeof(uint16_t) * 1);

   return PUD_TRUE;
}

PUDAPI Pud_Bool
pud_parse_slbr(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, PUD_FALSE);

   uint32_t chk;
   uint16_t buf[16];

   chk = pud_go_to_section(pud, PUD_SECTION_SLBR);
   if (!chk) DIE_RETURN(PUD_FALSE, "Failed to reach section SLBR");
   PUD_VERBOSE(pud, 2, "At section SLBR (size = %u)", chk);
   HAS_SECTION(PUD_SECTION_SLBR);

   PUD_TRAP_SETUP(pud) { return PUD_FALSE; }
   PUD_READ_BUFFER(pud, buf, sizeof(uint16_t) * 16);

   memcpy(&(pud->slbr.players[0]),  &(buf[0]),  sizeof(uint16_t) * 8);
   memcpy(&(pud->slbr.unusable[0]), &(buf[8]),  sizeof(uint16_t) * 7);
   memcpy(&(pud->slbr.neutral),     &(buf[15]), sizeof(uint16_t) * 1);

   return PUD_TRUE;
}

PUDAPI Pud_Bool
pud_parse_soil(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, PUD_FALSE);

   uint32_t chk;
   uint16_t buf[16];

   chk = pud_go_to_section(pud, PUD_SECTION_SOIL);
   if (!chk) DIE_RETURN(PUD_FALSE, "Failed to reach section SOIL");
   PUD_VERBOSE(pud, 2, "At section SOIL (size = %u)", chk);
   HAS_SECTION(PUD_SECTION_SOIL);

   PUD_TRAP_SETUP(pud) { return PUD_FALSE; }
   PUD_READ_BUFFER(pud, buf, sizeof(uint16_t) * 16);

   memcpy(&(pud->soil.players[0]),  &(buf[0]),  sizeof(uint16_t) * 8);
   memcpy(&(pud->soil.unusable[0]), &(buf[8]),  sizeof(uint16_t) * 7);
   memcpy(&(pud->soil.neutral),     &(buf[15]), sizeof(uint16_t) * 1);

   return PUD_TRUE;
}

PUDAPI Pud_Bool
pud_parse_aipl(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, PUD_FALSE);

   uint32_t chk;
   uint8_t buf[16];

   chk = pud_go_to_section(pud, PUD_SECTION_AIPL);
   if (!chk) DIE_RETURN(PUD_FALSE, "Failed to reach section AIPL");
   PUD_VERBOSE(pud, 2, "At section AIPL (size = %u)", chk);
   HAS_SECTION(PUD_SECTION_AIPL);

   PUD_READ_BUFFER(pud, buf, sizeof(uint8_t) * 16);

   memcpy(&(pud->ai.players[0]),  &(buf[0]),  sizeof(uint8_t) * 8);
   memcpy(&(pud->ai.unusable[0]), &(buf[8]),  sizeof(uint8_t) * 7);
   memcpy(&(pud->ai.neutral),     &(buf[15]), sizeof(uint8_t) * 1);

   return PUD_TRUE;
}

PUDAPI Pud_Bool
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
        w = PUD_READ16(pud);
        pud->tiles_map[i] = w;
     }

   return PUD_TRUE;
}

PUDAPI Pud_Bool
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
        w = PUD_READ16(pud);
        pud->movement_map[i] = w;
     }

   return PUD_TRUE;
}

PUDAPI Pud_Bool
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
             b = PUD_READ8(pud);
             pud->oil_map[i] = b;
          }
     }

   return PUD_TRUE;
}

PUDAPI Pud_Bool
pud_parse_regm(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, PUD_FALSE);

   uint32_t chk;
   uint16_t w;
   unsigned int i;

   PUD_TRAP_SETUP(pud) { return PUD_FALSE; }

   chk = pud_go_to_section(pud, PUD_SECTION_REGM);
   if (!chk) DIE_RETURN(PUD_FALSE, "Failed to reach section REGM");
   PUD_VERBOSE(pud, 2, "At section REGM (size = %u)", chk);
   HAS_SECTION(PUD_SECTION_REGM);

   /* Check for integrity */
   if ((pud->tiles * sizeof(uint16_t)) != chk)
     DIE_RETURN(PUD_FALSE, "Mismatch between dims and tiles number");

   for (i = 0; i < pud->tiles; i++)
     {
        w = PUD_READ16(pud);
        pud->action_map[i] = w;
     }

   return PUD_TRUE;
}

PUDAPI Pud_Bool
pud_parse_unit(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, PUD_FALSE);

   uint32_t chk;
   int units, size, i;
   Pud_Unit_Info *u;

   chk = pud_go_to_section(pud, PUD_SECTION_UNIT);
   if (!chk) DIE_RETURN(PUD_FALSE, "Failed to reach section UNIT");
   PUD_VERBOSE(pud, 2, "At section UNIT (size = %u)", chk);
   HAS_SECTION(PUD_SECTION_UNIT);
   units = chk / 8;

   size = sizeof(Pud_Unit_Info) * units;
   pud->units = realloc(pud->units, size);
   if (!pud->units) DIE_RETURN(PUD_FALSE, "Failed to allocate memory");
   memset(pud->units, 0, size);
   pud->units_count = units;

   for (i = 0; i < units; ++i)
     {
        pud->units[i].x      = PUD_READ16(pud);
        pud->units[i].y      = PUD_READ16(pud);
        pud->units[i].type   = PUD_READ8(pud);
        pud->units[i].player = PUD_READ8(pud);
        pud->units[i].alter  = PUD_READ16(pud);
     }

   pud->starting_points = 0;
   for (i = 0; i < units; i++)
     {
        u = &(pud->units[i]);

        /* Update data about units (for validity checks) */
        // TODO owner <-> player
        //if (owner == PUD_OWNER_HUMAN)
        //  pud->human_players++;
        //else if (owner == PUD_OWNER_COMPUTER)
        //  pud->computer_players++;
        if ((u->type == PUD_UNIT_HUMAN_START) ||
            (u->type == PUD_UNIT_ORC_START))
          pud->starting_points++;
     }

   return PUD_TRUE;
}
