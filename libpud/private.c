/*
 * private.c
 * libpud
 *
 * Copyright (c) 2014 Jean Guyomarc'h
 */

#include "pud_private.h"

const char *
long2bin(uint32_t x)
{
   static char buf[64];
   char b[16];
   int i, k = 0;
   const char *q;

   snprintf(b, sizeof(b), "%08x", x);
   for (i = 0; i < 8; i++)
     {
        switch (b[i])
          {
           case '0': q = "0000"; break;
           case '1': q = "0001"; break;
           case '2': q = "0010"; break;
           case '3': q = "0011"; break;
           case '4': q = "0100"; break;
           case '5': q = "0101"; break;
           case '6': q = "0110"; break;
           case '7': q = "0111"; break;
           case '8': q = "1000"; break;
           case '9': q = "1001"; break;
           case 'a': q = "1010"; break;
           case 'b': q = "1011"; break;
           case 'c': q = "1100"; break;
           case 'd': q = "1101"; break;
           case 'e': q = "1110"; break;
           case 'f': q = "1111"; break;
           default: return "<ERROR>";
          }
        memcpy(buf + k, q, 4);
        k += 4;
     }
   buf[k] = 0;

   return buf;
}

const char *
mode2str(Pud_Open_Mode mode)
{
   switch (mode)
     {
      case PUD_OPEN_MODE_R:  return "rb";
      case PUD_OPEN_MODE_W:  return "wb";
      default:               return NULL;
     }
}

Pud_Color
color_make(uint8_t r,
           uint8_t g,
           uint8_t b,
           uint8_t a)
{
   Pud_Color c = {
      .r = r,
      .g = g,
      .b = b,
      .a = a
   };
   return c;
}

const char *
dim2str(Pud_Dimensions dim)
{
   switch (dim)
     {
      case PUD_DIMENSIONS_32_32:   return "32 x 32";
      case PUD_DIMENSIONS_64_64:   return "64 x 64";
      case PUD_DIMENSIONS_96_96:   return "96 x 96";
      case PUD_DIMENSIONS_128_128: return "128 x 128";
      default: break;
     }
   return "<INVALID DIMENSIONS>";
}

const char *
color2str(Pud_Player color)
{
   switch (color)
   {
      case PUD_PLAYER_RED   : return "red";
      case PUD_PLAYER_BLUE  : return "blue";
      case PUD_PLAYER_GREEN : return "green";
      case PUD_PLAYER_VIOLET: return "violet";
      case PUD_PLAYER_ORANGE: return "orange";
      case PUD_PLAYER_BLACK : return "black";
      case PUD_PLAYER_WHITE : return "white";
      case PUD_PLAYER_YELLOW: return "yellow";
   }
   return "<INVALID COLOR>";
}

const char *
era2str(Pud_Era era)
{
   switch (era)
   {
      case PUD_ERA_FOREST:    return "Forest";
      case PUD_ERA_WINTER:    return "Winter";
      case PUD_ERA_WASTELAND: return "Wasteland";
      case PUD_ERA_SWAMP:     return "Swamp";
   }

   return "<INVALID ERA>";
}

Pud_Color
color_for_player(uint8_t player)
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
      default: ERR("Invalid player [%i]", player); break;
      }

      return color_make(0x7f, 0x7f, 0x7f, 0xff);
   }

   Pud_Bool
   pud_mem_map_ok(Pud *pud)
   {
      return (pud->ptr < pud->mem_map + pud->mem_map_size);
   }

   const char *
   unit2str(Pud_Unit unit)
   {
      switch (unit)
      {
         case PUD_UNIT_INFANTRY              : return "footman";
         case PUD_UNIT_GRUNT                 : return "grunt";
         case PUD_UNIT_PEASANT               : return "peasant";
         case PUD_UNIT_PEON                  : return "peon";
         case PUD_UNIT_BALLISTA              : return "ballista";
         case PUD_UNIT_CATAPULT              : return "catapult";
         case PUD_UNIT_KNIGHT                : return "knight";
         case PUD_UNIT_OGRE                  : return "ogre";
         case PUD_UNIT_ARCHER                : return "archer";
         case PUD_UNIT_AXETHROWER            : return "axethrower";
         case PUD_UNIT_MAGE                  : return "mage";
         case PUD_UNIT_DEATH_KNIGHT          : return "death_knight";
         case PUD_UNIT_PALADIN               : return "paladin";
         case PUD_UNIT_OGRE_MAGE             : return "ogre_mage";
         case PUD_UNIT_DWARVES               : return "dwarves";
         case PUD_UNIT_GOBLIN_SAPPER         : return "goblin_sapper";
         case PUD_UNIT_ATTACK_PEASANT        : return "attack_peasant";
         case PUD_UNIT_ATTACK_PEON           : return "attack_peon";
         case PUD_UNIT_RANGER                : return "ranger";
         case PUD_UNIT_BERSERKER             : return "berserker";
         case PUD_UNIT_ALLERIA               : return "alleria";
         case PUD_UNIT_TERON_GOREFIEND       : return "teron_gorefiend";
         case PUD_UNIT_KURDAN_AND_SKY_REE    : return "kurdan_and_sky_ree";
         case PUD_UNIT_DENTARG               : return "dentarg";
         case PUD_UNIT_KHADGAR               : return "khadgar";
         case PUD_UNIT_GROM_HELLSCREAM       : return "grom_hellscream";
         case PUD_UNIT_HUMAN_TANKER          : return "human_tanker";
         case PUD_UNIT_ORC_TANKER            : return "orc_tanker";
         case PUD_UNIT_HUMAN_TRANSPORT       : return "human_transport";
         case PUD_UNIT_ORC_TRANSPORT         : return "orc_transport";
         case PUD_UNIT_ELVEN_DESTROYER       : return "elven_destroyer";
         case PUD_UNIT_TROLL_DESTROYER       : return "troll_destroyer";
         case PUD_UNIT_BATTLESHIP            : return "battleship";
         case PUD_UNIT_JUGGERNAUGHT          : return "juggernaught";
         case PUD_UNIT_DEATHWING             : return "deathwing";
         case PUD_UNIT_GNOMISH_SUBMARINE     : return "gnomish_submarine";
         case PUD_UNIT_GIANT_TURTLE          : return "giant_turtle";
         case PUD_UNIT_GNOMISH_FLYING_MACHINE: return "gnomish_flying_machine";
         case PUD_UNIT_GOBLIN_ZEPPLIN        : return "goblin_zepplin";
         case PUD_UNIT_GRYPHON_RIDER         : return "gryphon_rider";
         case PUD_UNIT_DRAGON                : return "dragon";
         case PUD_UNIT_TURALYON              : return "turalyon";
         case PUD_UNIT_EYE_OF_KILROGG        : return "eye_of_kilrogg";
         case PUD_UNIT_DANATH                : return "danath";
         case PUD_UNIT_KHORGATH_BLADEFIST    : return "khorgath_bladefist";
         case PUD_UNIT_CHO_GALL              : return "cho_gall";
         case PUD_UNIT_LOTHAR                : return "lothar";
         case PUD_UNIT_GUL_DAN               : return "gul_dan";
         case PUD_UNIT_UTHER_LIGHTBRINGER    : return "uther_lightbringer";
         case PUD_UNIT_ZULJIN                : return "zuljin";
         case PUD_UNIT_SKELETON              : return "skeleton";
         case PUD_UNIT_DAEMON                : return "daemon";
         case PUD_UNIT_CRITTER               : return "critter";
         case PUD_UNIT_FARM                  : return "farm";
         case PUD_UNIT_PIG_FARM              : return "pig_farm";
         case PUD_UNIT_HUMAN_BARRACKS        : return "human_barracks";
         case PUD_UNIT_ORC_BARRACKS          : return "orc_barracks";
         case PUD_UNIT_CHURCH                : return "church";
         case PUD_UNIT_ALTAR_OF_STORMS       : return "altar_of_storms";
         case PUD_UNIT_HUMAN_SCOUT_TOWER     : return "human_scout_tower";
         case PUD_UNIT_ORC_SCOUT_TOWER       : return "orc_scout_tower";
         case PUD_UNIT_STABLES               : return "stables";
         case PUD_UNIT_OGRE_MOUND            : return "ogre_mound";
         case PUD_UNIT_GNOMISH_INVENTOR      : return "gnomish_inventor";
         case PUD_UNIT_GOBLIN_ALCHEMIST      : return "goblin_alchemist";
         case PUD_UNIT_GRYPHON_AVIARY        : return "gryphon_aviary";
         case PUD_UNIT_DRAGON_ROOST          : return "dragon_roost";
         case PUD_UNIT_HUMAN_SHIPYARD        : return "human_shipyard";
         case PUD_UNIT_ORC_SHIPYARD          : return "orc_shipyard";
         case PUD_UNIT_TOWN_HALL             : return "town_hall";
         case PUD_UNIT_GREAT_HALL            : return "great_hall";
         case PUD_UNIT_ELVEN_LUMBER_MILL     : return "elven_lumber_mill";
         case PUD_UNIT_TROLL_LUMBER_MILL     : return "troll_lumber_mill";
         case PUD_UNIT_HUMAN_FOUNDRY         : return "human_foundry";
         case PUD_UNIT_ORC_FOUNDRY           : return "orc_foundry";
         case PUD_UNIT_MAGE_TOWER            : return "mage_tower";
         case PUD_UNIT_TEMPLE_OF_THE_DAMNED  : return "temple_of_the_damned";
         case PUD_UNIT_HUMAN_BLACKSMITH      : return "human_blacksmith";
         case PUD_UNIT_ORC_BLACKSMITH        : return "orc_blacksmith";
         case PUD_UNIT_HUMAN_REFINERY        : return "human_refinery";
         case PUD_UNIT_ORC_REFINERY          : return "orc_refinery";
         case PUD_UNIT_HUMAN_OIL_WELL        : return "human_oil_well";
         case PUD_UNIT_ORC_OIL_WELL          : return "orc_oil_well";
         case PUD_UNIT_KEEP                  : return "keep";
         case PUD_UNIT_STRONGHOLD            : return "stronghold";
         case PUD_UNIT_CASTLE                : return "castle";
         case PUD_UNIT_FORTRESS              : return "fortress";
         case PUD_UNIT_GOLD_MINE             : return "gold_mine";
         case PUD_UNIT_OIL_PATCH             : return "oil_patch";
         case PUD_UNIT_HUMAN_START           : return "human_start";
         case PUD_UNIT_ORC_START             : return "orc_start";
         case PUD_UNIT_HUMAN_GUARD_TOWER     : return "human_guard_tower";
         case PUD_UNIT_ORC_GUARD_TOWER       : return "orc_guard_tower";
         case PUD_UNIT_HUMAN_CANNON_TOWER    : return "human_cannon_tower";
         case PUD_UNIT_ORC_CANNON_TOWER      : return "orc_cannon_tower";
         case PUD_UNIT_CIRCLE_OF_POWER       : return "circle_of_power";
         case PUD_UNIT_DARK_PORTAL           : return "dark_portal";
         case PUD_UNIT_RUNESTONE             : return "runestone";
         case PUD_UNIT_HUMAN_WALL            : return "human_wall";
         case PUD_UNIT_ORC_WALL              : return "orc_wall";
         case PUD_UNIT_CRITTER_SHEEP         : return "sheep";
         case PUD_UNIT_CRITTER_PIG           : return "pig";
         case PUD_UNIT_CRITTER_SEAL          : return "seal";
         default                             : return "<UNKNOWN UNIT>";
      }
   }

