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
                       unsigned int   *x_ret,
                       unsigned int   *y_ret)
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
      default:                return "<INVALID COLOR>";
   }
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

typedef struct
{
   const char *name;
   const char *pretty;
} Unit;

static const Unit _names[] =
{
   [PUD_UNIT_FOOTMAN] = {
      .name =  "footman",
      .pretty = "Footman",
   },
   [PUD_UNIT_GRUNT] = {
      .name =  "grunt",
      .pretty = "Grunt",
   },
   [PUD_UNIT_PEASANT] = {
      .name =  "peasant",
      .pretty = "Peasant",
   },
   [PUD_UNIT_PEON] = {
      .name =  "peon",
      .pretty = "Peon",
   },
   [PUD_UNIT_BALLISTA] = {
      .name =  "ballista",
      .pretty =  "Ballista",
   },
   [PUD_UNIT_CATAPULT] = {
      .name =  "catapult",
      .pretty =  "Catapult",
   },
   [PUD_UNIT_KNIGHT] = {
      .name =  "knight",
      .pretty =  "Knight",
   },
   [PUD_UNIT_OGRE] = {
      .name =  "ogre",
      .pretty =  "Ogre",
   },
   [PUD_UNIT_ARCHER] = {
      .name =  "archer",
      .pretty =  "Archer",
   },
   [PUD_UNIT_AXETHROWER] = {
      .name =  "axethrower",
      .pretty =  "Axethrower",
   },
   [PUD_UNIT_MAGE] = {
      .name =  "mage",
      .pretty =  "Mage",
   },
   [PUD_UNIT_DEATH_KNIGHT] = {
      .name =  "death_knight",
      .pretty =  "Death Knight",
   },
   [PUD_UNIT_PALADIN] = {
      .name =  "paladin",
      .pretty =  "Paladin",
   },
   [PUD_UNIT_OGRE_MAGE] = {
      .name =  "ogre_mage",
      .pretty =  "Ogre Mage",
   },
   [PUD_UNIT_DWARVES] = {
      .name =  "dwarves",
      .pretty =  "Dwarven Demolition Squad",
   },
   [PUD_UNIT_GOBLIN_SAPPER] = {
      .name =  "goblin_sapper",
      .pretty =  "Goblin Sapper",
   },
   [PUD_UNIT_ATTACK_PEASANT] = {
      .name =  "attack_peasant",
      .pretty =  "Peasant (Attack)",
   },
   [PUD_UNIT_ATTACK_PEON] = {
      .name =  "attack_peon",
      .pretty =  "Peon (Attack)",
   },
   [PUD_UNIT_RANGER] = {
      .name =  "ranger",
      .pretty =  "Elven Ranger",
   },
   [PUD_UNIT_BERSERKER] = {
      .name =  "berserker",
      .pretty =  "Berserker",
   },
   [PUD_UNIT_ALLERIA] = {
      .name =  "alleria",
      .pretty =  "Alleria",
   },
   [PUD_UNIT_TERON_GOREFIEND] = {
      .name =  "teron_gorefiend",
      .pretty =  "Teron Gorefiend",
   },
   [PUD_UNIT_KURDRAN_AND_SKY_REE] = {
      .name =  "kurdran_and_sky_ree",
      .pretty =  "Kurdran and Sky'ree",
   },
   [PUD_UNIT_DENTARG] = {
      .name =  "dentarg",
      .pretty =  "Dentarg",
   },
   [PUD_UNIT_KHADGAR] = {
      .name =  "khadgar",
      .pretty =  "Khadgar",
   },
   [PUD_UNIT_GROM_HELLSCREAM] = {
      .name =  "grom_hellscream",
      .pretty =  "Grom Hellscream",
   },
   [PUD_UNIT_HUMAN_TANKER] = {
      .name =  "human_tanker",
      .pretty =  "Human Tanker",
   },
   [PUD_UNIT_ORC_TANKER] = {
      .name =  "orc_tanker",
      .pretty =  "Orc Tanker",
   },
   [PUD_UNIT_HUMAN_TRANSPORT] = {
      .name =  "human_transport",
      .pretty = "Human Transport",
   },
   [PUD_UNIT_ORC_TRANSPORT] = {
      .name =  "orc_transport",
      .pretty =  "Orc Transport",
   },
   [PUD_UNIT_ELVEN_DESTROYER] = {
      .name =  "elven_destroyer",
      .pretty =  "Elven Destroyer",
   },
   [PUD_UNIT_TROLL_DESTROYER] = {
      .name =  "troll_destroyer",
      .pretty =  "Troll Destroyer",
   },
   [PUD_UNIT_BATTLESHIP] = {
      .name =  "battleship",
      .pretty =  "Battleship",
   },
   [PUD_UNIT_JUGGERNAUGHT] = {
      .name =  "juggernaught",
      .pretty =  "Juggernaught",
   },
   [PUD_UNIT_DEATHWING] = {
      .name =  "deathwing",
      .pretty =  "Deathwing",
   },
   [PUD_UNIT_GNOMISH_SUBMARINE] = {
      .name =  "gnomish_submarine",
      .pretty =  "Gnomish Submarine",
   },
   [PUD_UNIT_GIANT_TURTLE] = {
      .name =  "giant_turtle",
      .pretty =  "Giant Turtle",
   },
   [PUD_UNIT_GNOMISH_FLYING_MACHINE] = {
      .name =  "gnomish_flying_machine",
      .pretty =  "Gnomish Flying Machine",
   },
   [PUD_UNIT_GOBLIN_ZEPPLIN] = {
      .name =  "goblin_zepplin",
      .pretty =  "Goblin Zepplin",
   },
   [PUD_UNIT_GRYPHON_RIDER] = {
      .name =  "gryphon_rider",
      .pretty =  "Gryphon Rider",
   },
   [PUD_UNIT_DRAGON] = {
      .name =  "dragon",
      .pretty =  "Dragon",
   },
   [PUD_UNIT_TURALYON] = {
      .name =  "turalyon",
      .pretty =  "Turalyon",
   },
   [PUD_UNIT_EYE_OF_KILROGG] = {
      .name =  "eye_of_kilrogg",
      .pretty =  "Eye of Kilrogg",
   },
   [PUD_UNIT_DANATH] = {
      .name =  "danath",
      .pretty =  "Danath",
   },
   [PUD_UNIT_KARGATH_BLADEFIST] = {
      .name =  "kargath_bladefist",
      .pretty =  "Kargath Bladefist",
   },
   [PUD_UNIT_CHO_GALL] = {
      .name =  "cho_gall",
      .pretty =  "Cho'gall",
   },
   [PUD_UNIT_LOTHAR] = {
      .name =  "lothar",
      .pretty =  "Lothar",
   },
   [PUD_UNIT_GUL_DAN] = {
      .name =  "gul_dan",
      .pretty =  "Gul'dan",
   },
   [PUD_UNIT_UTHER_LIGHTBRINGER] = {
      .name =  "uther_lightbringer",
      .pretty =  "Uther Lightbringer",
   },
   [PUD_UNIT_ZUL_JIN] = {
      .name =  "zul_jin",
      .pretty =  "Zul'jin",
   },
   [PUD_UNIT_SKELETON] = {
      .name =  "skeleton",
      .pretty =  "Skeleton",
   },
   [PUD_UNIT_DAEMON] = {
      .name =  "daemon",
      .pretty =  "Daemon",
   },
   [PUD_UNIT_CRITTER] = {
      .name =  "critter",
      .pretty =  "Critter",
   },
   [PUD_UNIT_FARM] = {
      .name =  "farm",
      .pretty =  "Farm",
   },
   [PUD_UNIT_PIG_FARM] = {
      .name =  "pig_farm",
      .pretty =  "Pig Farm",
   },
   [PUD_UNIT_HUMAN_BARRACKS] = {
      .name =  "human_barracks",
      .pretty =  "Human Barracks",
   },
   [PUD_UNIT_ORC_BARRACKS] = {
      .name =  "orc_barracks",
      .pretty =  "Orc Barracks",
   },
   [PUD_UNIT_CHURCH] = {
      .name =  "church",
      .pretty =  "Church",
   },
   [PUD_UNIT_ALTAR_OF_STORMS] = {
      .name =  "altar_of_storms",
      .pretty =  "Altar of Storms",
   },
   [PUD_UNIT_HUMAN_SCOUT_TOWER] = {
      .name =  "human_scout_tower",
      .pretty =  "Human Scout_Tower",
   },
   [PUD_UNIT_ORC_SCOUT_TOWER] = {
      .name =  "orc_scout_tower",
      .pretty =  "Orc Scout Tower",
   },
   [PUD_UNIT_STABLES] = {
      .name =  "stables",
      .pretty =  "Stables",
   },
   [PUD_UNIT_OGRE_MOUND] = {
      .name =  "ogre_mound",
      .pretty =  "Ogre Mound",
   },
   [PUD_UNIT_GNOMISH_INVENTOR] = {
      .name =  "gnomish_inventor",
      .pretty =  "Gnomish Inventor",
   },
   [PUD_UNIT_GOBLIN_ALCHEMIST] = {
      .name =  "goblin_alchemist",
      .pretty =  "Goblin Alchemist",
   },
   [PUD_UNIT_GRYPHON_AVIARY] = {
      .name =  "gryphon_aviary",
      .pretty =  "Gryphon Aviary",
   },
   [PUD_UNIT_DRAGON_ROOST] = {
      .name =  "dragon_roost",
      .pretty =  "Dragon Roost",
   },
   [PUD_UNIT_HUMAN_SHIPYARD] = {
      .name =  "human_shipyard",
      .pretty =  "Human Shipyard",
   },
   [PUD_UNIT_ORC_SHIPYARD] = {
      .name =  "orc_shipyard",
      .pretty =  "Orc Shipyard",
   },
   [PUD_UNIT_TOWN_HALL] = {
      .name =  "town_hall",
      .pretty =  "Town Hall",
   },
   [PUD_UNIT_GREAT_HALL] = {
      .name =  "great_hall",
      .pretty =  "Great Hall",
   },
   [PUD_UNIT_ELVEN_LUMBER_MILL] = {
      .name =  "elven_lumber_mill",
      .pretty =  "Elven Lumber Mill",
   },
   [PUD_UNIT_TROLL_LUMBER_MILL] = {
      .name =  "troll_lumber_mill",
      .pretty =  "Troll Lumber Mill",
   },
   [PUD_UNIT_HUMAN_FOUNDRY] = {
      .name =  "human_foundry",
      .pretty =  "Human Foundry",
   },
   [PUD_UNIT_ORC_FOUNDRY] = {
      .name =  "orc_foundry",
      .pretty =  "Orc Foundry",
   },
   [PUD_UNIT_MAGE_TOWER] = {
      .name =  "mage_tower",
      .pretty =  "Mage Tower",
   },
   [PUD_UNIT_TEMPLE_OF_THE_DAMNED] = {
      .name =  "temple_of_the_damned",
      .pretty =  "Temple of the Damned",
   },
   [PUD_UNIT_HUMAN_BLACKSMITH] = {
      .name =  "human_blacksmith",
      .pretty =  "Human Blacksmith",
   },
   [PUD_UNIT_ORC_BLACKSMITH] = {
      .name =  "orc_blacksmith",
      .pretty =  "Orc Blacksmith",
   },
   [PUD_UNIT_HUMAN_REFINERY] = {
      .name =  "human_refinery",
      .pretty =  "Human Refinery",
   },
   [PUD_UNIT_ORC_REFINERY] = {
      .name =  "orc_refinery",
      .pretty =  "Orc Refinery",
   },
   [PUD_UNIT_HUMAN_OIL_WELL] = {
      .name =  "human_oil_well",
      .pretty =  "Human Oil Well",
   },
   [PUD_UNIT_ORC_OIL_WELL] = {
      .name =  "orc_oil_well",
      .pretty =  "Orc Oil Well",
   },
   [PUD_UNIT_KEEP] = {
      .name =  "keep",
      .pretty =  "Keep",
   },
   [PUD_UNIT_STRONGHOLD] = {
      .name =  "stronghold",
      .pretty = "Stronghold",
   },
   [PUD_UNIT_CASTLE] = {
      .name =  "castle",
      .pretty = "Castle",
   },
   [PUD_UNIT_FORTRESS] = {
      .name =  "fortress",
      .pretty = "Fortress",
   },
   [PUD_UNIT_GOLD_MINE] = {
      .name =  "gold_mine",
      .pretty = "Gold Mine",
   },
   [PUD_UNIT_OIL_PATCH] = {
      .name =  "oil_patch",
      .pretty = "Oil Patch",
   },
   [PUD_UNIT_HUMAN_START] = {
      .name =  "human_start",
      .pretty = "Human Start Location",
   },
   [PUD_UNIT_ORC_START] = {
      .name =  "orc_start",
      .pretty = "Orc Start Location",
   },
   [PUD_UNIT_HUMAN_GUARD_TOWER] = {
      .name =  "human_guard_tower",
      .pretty = "Human Guard Tower",
   },
   [PUD_UNIT_ORC_GUARD_TOWER] = {
      .name =  "orc_guard_tower",
      .pretty = "Orc Guard Tower",
   },
   [PUD_UNIT_HUMAN_CANNON_TOWER] = {
      .name =  "human_cannon_tower",
      .pretty = "Human Cannon Tower",
   },
   [PUD_UNIT_ORC_CANNON_TOWER] = {
      .name =  "orc_cannon_tower",
      .pretty = "Orc Cannon Tower",
   },
   [PUD_UNIT_CIRCLE_OF_POWER] = {
      .name =  "circle_of_power",
      .pretty = "Circle of Power",
   },
   [PUD_UNIT_DARK_PORTAL] = {
      .name =  "dark_portal",
      .pretty = "Dark Portal",
   },
   [PUD_UNIT_RUNESTONE] = {
      .name =  "runestone",
      .pretty = "Runestone",
   },
   [PUD_UNIT_HUMAN_WALL] = {
      .name =  "human_wall",
      .pretty = "Human Wall",
   },
   [PUD_UNIT_ORC_WALL] = {
      .name =  "orc_wall",
      .pretty = "Orc Wall",
   },
   [PUD_UNIT_CRITTER_SHEEP] = {
      .name =  "sheep",
      .pretty = "Sheep",
   },
   [PUD_UNIT_CRITTER_PIG] = {
      .name =  "pig",
      .pretty = "Pid",
   },
   [PUD_UNIT_CRITTER_SEAL] = {
      .name =  "seal",
      .pretty = "Seal",
   },
   [PUD_UNIT_CRITTER_RED_PIG] = {
      .name =  "red_pig",
      .pretty = "Red Pig",
   },
};

const char *
pud_unit2str(Pud_Unit unit,
             Pud_Bool pretty)
{
   if ((unit < PUD_UNIT_FOOTMAN) && (unit > PUD_UNIT_CRITTER_RED_PIG))
     return NULL;
   return (pretty) ? _names[unit].pretty : _names[unit].name;
}
