/*
 * print.c
 * libpud
 *
 * Copyright (c) 2014 Jean Guyomarc'h
 */

#include "pud_private.h"

/* It is such a heavy function I put it in a single file */

void
pud_print(Pud  *pud,
          FILE *stream)
{
   unsigned int i, j;

   if (!stream) stream = stdout;

   fprintf(stream, "Tag ID...............: 0x%x\n", pud->tag);
   fprintf(stream, "Version..............: %x\n", pud->version);
   fprintf(stream, "Description..........: %s\n", pud->description);
   fprintf(stream, "Era..................: %s\n", pud_era2str(pud->era));
   fprintf(stream, "Dimensions...........: %s\n", dim2str(pud->dims));
   fprintf(stream, "Default ALOW.........: %i\n", pud->default_allow);
   fprintf(stream, "Default UDTA.........: %i\n", pud->default_udta);
   fprintf(stream, "Default UGRD.........: %i\n", pud->default_ugrd);

   /* OWNR Section */
   fprintf(stream, "Owners...............:\n");
   for (i = 0; i < 8; i++)
     fprintf(stream, "   player %i..........: 0x%02x\n", i + 1, pud->owner.players[i]);
   for (i = 0; i < 7; i++)
     fprintf(stream, "   unusable %i........: 0x%02x\n", i + 1, pud->owner.unusable[i]);
   fprintf(stream, "   neutral...........: 0x%02x\n", pud->owner.neutral);

   /* Side Section */
   fprintf(stream, "Sides................:\n");
   for (i = 0; i < 8; i++)
     fprintf(stream, "   player %i..........: 0x%02x\n", i + 1, pud->side.players[i]);
   for (i = 0; i < 7; i++)
     fprintf(stream, "   unusable %i........: 0x%02x\n", i + 1, pud->side.unusable[i]);
   fprintf(stream, "   neutral...........: 0x%02x\n", pud->side.neutral);

   /* SGLD Section */
   fprintf(stream, "Starting Gold........:\n");
   for (i = 0; i < 8; i++)
     fprintf(stream, "   player %i..........: %u\n", i + 1, pud->sgld.players[i]);
   for (i = 0; i < 7; i++)
     fprintf(stream, "   unusable %i........: %u\n", i + 1, pud->sgld.unusable[i]);
   fprintf(stream, "   neutral...........: %u\n", pud->sgld.neutral);

   /* SLBR section */
   fprintf(stream, "Starting Lumber......:\n");
   for (i = 0; i < 8; i++)
     fprintf(stream, "   player %i..........: %u\n", i + 1, pud->slbr.players[i]);
   for (i = 0; i < 7; i++)
     fprintf(stream, "   unusable %i........: %u\n", i + 1, pud->slbr.unusable[i]);
   fprintf(stream, "   neutral...........: %u\n", pud->slbr.neutral);

   /* SOIL section */
   fprintf(stream, "Starting Oil.........:\n");
   for (i = 0; i < 8; i++)
     fprintf(stream, "   player %i..........: %u\n", i + 1, pud->soil.players[i]);
   for (i = 0; i < 7; i++)
     fprintf(stream, "   unusable %i........: %u\n", i + 1, pud->soil.unusable[i]);
   fprintf(stream, "   neutral...........: %u\n", pud->soil.neutral);

   /* AI section */
   fprintf(stream, "AI...................:\n");
   for (i = 0; i < 8; i++)
     fprintf(stream, "   player %i..........: 0x%02x\n", i + 1, pud->ai.players[i]);
   for (i = 0; i < 7; i++)
     fprintf(stream, "   unusable %i........: 0x%02x\n", i + 1, pud->ai.unusable[i]);
   fprintf(stream, "   neutral...........: 0x%02x\n", pud->ai.neutral);

   /* ALOW section - Units & Buildings */
   fprintf(stream, "Allow Units..........:\n");
   for (i = 0; i < 8; i++)
     fprintf(stream, "   player %i..........: %s\n", i + 1, long2bin(pud->unit_alow.players[i]));
   for (i = 0; i < 7; i++)
     fprintf(stream, "   unusable %i........: %s\n", i + 1, long2bin(pud->unit_alow.unusable[i]));
   fprintf(stream, "   neutral...........: %s\n", long2bin(pud->unit_alow.neutral));

   /* ALOW section - Startup Spells */
   fprintf(stream, "Startup Spells.......:\n");
   for (i = 0; i < 8; i++)
     fprintf(stream, "   player %i..........: %s\n", i + 1, long2bin(pud->spell_start.players[i]));
   for (i = 0; i < 7; i++)
     fprintf(stream, "   unusable %i........: %s\n", i + 1, long2bin(pud->spell_start.unusable[i]));
   fprintf(stream, "   neutral...........: %s\n", long2bin(pud->spell_start.neutral));

   /* ALOW section - Spells Allowed */
   fprintf(stream, "Allow Spells.........:\n");
   for (i = 0; i < 8; i++)
     fprintf(stream, "   player %i..........: %s\n", i + 1, long2bin(pud->spell_alow.players[i]));
   for (i = 0; i < 7; i++)
     fprintf(stream, "   unusable %i........: %s\n", i + 1, long2bin(pud->spell_alow.unusable[i]));
   fprintf(stream, "   neutral...........: %s\n", long2bin(pud->spell_alow.neutral));

   /* ALOW section - Spells Researching */
   fprintf(stream, "Searching Spells.....:\n");
   for (i = 0; i < 8; i++)
     fprintf(stream, "   player %i..........: %s\n", i + 1, long2bin(pud->spell_alow.players[i]));
   for (i = 0; i < 7; i++)
     fprintf(stream, "   unusable %i........: %s\n", i + 1, long2bin(pud->spell_alow.unusable[i]));
   fprintf(stream, "   neutral...........: %s\n", long2bin(pud->spell_alow.neutral));

   /* ALOW section - Upgrades allowed */
   fprintf(stream, "Upgrades Allowed.....:\n");
   for (i = 0; i < 8; i++)
     fprintf(stream, "   player %i..........: %s\n", i + 1, long2bin(pud->up_alow.players[i]));
   for (i = 0; i < 7; i++)
     fprintf(stream, "   unusable %i........: %s\n", i + 1, long2bin(pud->up_alow.unusable[i]));
   fprintf(stream, "   neutral...........: %s\n", long2bin(pud->up_alow.neutral));

   /* ALOW section - Upgrades Researching */
   fprintf(stream, "Searching Upgrades...:\n");
   for (i = 0; i < 8; i++)
     fprintf(stream, "   player %i..........: %s\n", i + 1, long2bin(pud->up_acq.players[i]));
   for (i = 0; i < 7; i++)
     fprintf(stream, "   unusable %i........: %s\n", i + 1, long2bin(pud->up_acq.unusable[i]));
   fprintf(stream, "   neutral...........: %s\n", long2bin(pud->up_acq.neutral));

   /* UGRD Section */
   fprintf(stream, "Upgrades.............:\n");
   for (i = 0; i < 52; i++)
     {
        fprintf(stream, "   Upgrade 0x%02x......:\n", i);
        fprintf(stream, "      Time...........: %u\n", pud->upgrade[i].time);
        fprintf(stream, "      Gold...........: %u\n", pud->upgrade[i].gold);
        fprintf(stream, "      Lumber.........: %u\n", pud->upgrade[i].lumber);
        fprintf(stream, "      Oil............: %u\n", pud->upgrade[i].oil);
        fprintf(stream, "      Icon...........: %u\n", pud->upgrade[i].icon);
        fprintf(stream, "      Group..........: 0x%02x\n", pud->upgrade[i].group);
        fprintf(stream, "      Flags..........: %s\n", long2bin(pud->upgrade[i].flags));
     }

   /* Units */
   fprintf(stream, "Units................: %u\n", pud->units_count);
   for (i = 0; i < pud->units_count; i++)
     {
        fprintf(stream, "   Unit %04i.........:\n", i);
        fprintf(stream, "      X,Y............: %i,%i\n", pud->units[i].x, pud->units[i].y);
        fprintf(stream, "      Type...........: %s (0x%x)\n", pud_unit2str(pud->units[i].type, PUD_TRUE), pud->units[i].type);
        fprintf(stream, "      Owner..........: 0x%x\n", pud->units[i].owner);
        fprintf(stream, "      Alter..........: %u\n", pud->units[i].alter);
     }

   /* Unit data */
   fprintf(stream, "Unit Data............:\n");
   for (i = 0; i < 110; i++)
     {
        fprintf(stream, "   Unit 0x%02x.........: %s\n", i, pud_unit2str(i, PUD_TRUE));
        fprintf(stream, "      Overlap........: %x\n", pud->unit_data[i].overlap_frames);
        fprintf(stream, "      Sight..........: %u\n", pud->unit_data[i].sight);
        fprintf(stream, "      Hit Points.....: %u\n", pud->unit_data[i].hp);
        fprintf(stream, "      Build Time.....: %u\n", pud->unit_data[i].build_time);
        fprintf(stream, "      Gold Cost......: %u\n", pud->unit_data[i].gold_cost);
        fprintf(stream, "      Lumber Cost....: %u\n", pud->unit_data[i].lumber_cost);
        fprintf(stream, "      Oil Cost.......: %u\n", pud->unit_data[i].oil_cost);
        fprintf(stream, "      Width..........: %u\n", pud->unit_data[i].size_w);
        fprintf(stream, "      Height.........: %u\n", pud->unit_data[i].size_h);
        fprintf(stream, "      Box Width......: %u\n", pud->unit_data[i].box_w);
        fprintf(stream, "      Box Height.....: %u\n", pud->unit_data[i].box_h);
        fprintf(stream, "      Range..........: %u\n", pud->unit_data[i].range);
        fprintf(stream, "      Cptr react rg..: %u\n", pud->unit_data[i].computer_react_range);
        fprintf(stream, "      Hmn reac rg....: %u\n", pud->unit_data[i].human_react_range);
        fprintf(stream, "      Armor..........: %u\n", pud->unit_data[i].armor);
        fprintf(stream, "      Priority.......: %u\n", pud->unit_data[i].priority);
        fprintf(stream, "      Basic Dmg......: %u\n", pud->unit_data[i].basic_damage);
        fprintf(stream, "      Piercing Dmg...: %u\n", pud->unit_data[i].piercing_damage);
        fprintf(stream, "      Missile........: %u\n", pud->unit_data[i].missile_weapon);
        fprintf(stream, "      Type...........: %u\n", pud->unit_data[i].type);
        fprintf(stream, "      Decay Rate.....: %u\n", pud->unit_data[i].decay_rate);
        fprintf(stream, "      Annoy..........: %u\n", pud->unit_data[i].annoy);
        fprintf(stream, "      Mouse 2 Btn....: %u\n", pud->unit_data[i].mouse_right_btn);
        fprintf(stream, "      Point Value....: %u\n", pud->unit_data[i].point_value);
        fprintf(stream, "      Can Target.....: %u\n", pud->unit_data[i].can_target);
        fprintf(stream, "      Rect Sel.......: %i\n", pud->unit_data[i].rect_sel);
        fprintf(stream, "      Has Magic......: %i\n", pud->unit_data[i].has_magic);
        fprintf(stream, "      Weapons Ugrd...: %i\n", pud->unit_data[i].weapons_upgradable);
        fprintf(stream, "      Armor Ugrd.....: %i\n", pud->unit_data[i].armor_upgradable);
        fprintf(stream, "      Flags..........: %s\n", long2bin(pud->unit_data[i].flags));
     }

   /* Tiles map */
   fprintf(stream, "Tiles Map (%s)\n", dim2str(pud->dims));
   for (i = 0; i < pud->map_h; i++)
     {
        for (j = 0; j < pud->map_w; j++)
          {
             fprintf(stream, "0x%04x", pud->tiles_map[(i * pud->map_w) + j]);
             if (j < pud->map_w - 1)
               fprintf(stream, " ");
          }
        fprintf(stream, "\n");
     }

   /* Action map */
   fprintf(stream, "Action Map (%s)\n", dim2str(pud->dims));
   for (i = 0; i < pud->map_h; i++)
     {
        for (j = 0; j < pud->map_w; j++)
          {
             fprintf(stream, "0x%04x", pud->action_map[(i * pud->map_w) + j]);
             if (j < pud->map_w - 1)
               fprintf(stream, " ");
          }
        fprintf(stream, "\n");
     }

   /* Movement map */
   fprintf(stream, "Movement Map (%s)\n", dim2str(pud->dims));
   for (i = 0; i < pud->map_h; i++)
     {
        for (j = 0; j < pud->map_w; j++)
          {
             fprintf(stream, "0x%04x", pud->movement_map[(i * pud->map_w) + j]);
             if (j < pud->map_w - 1)
               fprintf(stream, " ");
          }
        fprintf(stream, "\n");
     }
}
