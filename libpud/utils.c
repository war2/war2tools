/*
 * utils.c
 * libpud
 *
 * Copyright (c) 2014 Jean Guyomarc'h
 */

#include "pud_private.h"

Pud_Owner
pud_owner_convert(uint8_t code)
{
   switch (code)
     {
      case 0x00:
      case 0x02:
      case 0x08 ... 0xff:
         return PUD_OWNER_PASSIVE_COMPUTER;

      case 0x01:
      case 0x04:
         return PUD_OWNER_COMPUTER;

      case 0x05:
         return PUD_OWNER_HUMAN;

      case 0x06:
         return PUD_OWNER_RESCUE_PASSIVE;

      case 0x07:
         return PUD_OWNER_RESCUE_ACTIVE;

      case 0x03:
      default:
         return PUD_OWNER_NOBODY;
     }
}

Pud_Side
pud_side_convert(uint8_t code)
{
   switch (code)
     {
      case 0x00:
         return PUD_SIDE_HUMAN;

      case 0x01:
         return PUD_SIDE_ORC;

      default:
         return PUD_SIDE_NEUTRAL;
     }
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

const char *
pud_color2str(Pud_Player color)
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
pud_era2str(Pud_Era era)
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

const char *
pud_unit2str(Pud_Unit unit)
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

