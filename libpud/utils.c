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
   const char *colors[] = {
      "red", "blue", "green", "violet",
      "orange", "black", "white", "yellow"
   };

   return ((unsigned)color > PUD_PLAYER_YELLOW) ? NULL : colors[color];
}

const char *
pud_era2str(Pud_Era era)
{
   const char *eras[] = {
      "Forest", "Winter", "Wasteland", "Swamp"
   };

   return ((unsigned)era > PUD_ERA_SWAMP) ? NULL : eras[era];
}

typedef struct
{
   const char *name;
   const char *pretty;
   Pud_Icon icon;
   Pud_Bool hero;
} Unit;

#define UNIT(suffix_, name_, pretty_) \
   [PUD_UNIT_ ## suffix_] = { \
      .name = name_, \
      .pretty = pretty_, \
      .icon = PUD_ICON_ ## suffix_, \
      .hero = PUD_FALSE, \
   }

#define HERO(suffix_, name_, pretty_) \
   [PUD_UNIT_ ## suffix_] = { \
      .name = name_, \
      .pretty = pretty_, \
      .icon = PUD_ICON_ ## suffix_, \
      .hero = PUD_TRUE, \
   }

#define UNIT_START_LOCATION(suffix_, name_, pretty_) \
   [PUD_UNIT_ ## suffix_] = { \
      .name = name_, \
      .pretty = pretty_, \
      .icon = PUD_ICON_CANCEL, \
      .hero = PUD_FALSE, \
   }

#define UNIT_START_LOCATION(suffix_, name_, pretty_) \
   [PUD_UNIT_ ## suffix_] = { \
      .name = name_, \
      .pretty = pretty_, \
      .icon = PUD_ICON_CANCEL, \
      .hero = PUD_FALSE, \
   }

static const Unit _names[110] =
{
   UNIT(FOOTMAN, "footman", "Footman"),
   UNIT(GRUNT, "grunt", "Grunt"),
   UNIT(PEASANT, "peasant", "Peasant"),
   UNIT(PEON, "peon", "Peon"),
   UNIT(BALLISTA, "ballista", "Ballista"),
   UNIT(CATAPULT, "catapult", "Catapult"),
   UNIT(KNIGHT, "knight", "Knight"),
   UNIT(OGRE, "ogre", "Ogre"),
   UNIT(ARCHER, "archer", "Archer"),
   UNIT(AXETHROWER, "axethrower", "Axethrower"),
   UNIT(MAGE, "mage", "Mage"),
   UNIT(DEATH_KNIGHT, "death_knight", "Death Knight"),
   UNIT(PALADIN, "paladin", "Paladin"),
   UNIT(OGRE_MAGE, "ogre_mage", "Ogre Mage"),
   UNIT(DWARVES, "dwarves", "Dwarven Demolition Squad"),
   UNIT(GOBLIN_SAPPER, "goblin_sapper", "Goblin Sapper"),
   UNIT(ATTACK_PEASANT, "attack_peasant", "Peasant (Attack)"),
   UNIT(ATTACK_PEON, "attack_peon", "Peon (Attack)"),
   UNIT(RANGER, "ranger", "Elven Ranger"),
   UNIT(BERSERKER, "berserker", "Berserker"),
   HERO(ALLERIA, "alleria", "Alleria"),
   HERO(TERON_GOREFIEND, "teron_gorefiend", "Teron Gorefiend"),
   HERO(KURDRAN_AND_SKY_REE, "kurdran_and_sky_ree", "Kurdran and Sky'ree"),
   HERO(DENTARG, "dentarg", "Dentarg"),
   HERO(KHADGAR, "khadgar", "Khadgar"),
   HERO(GROM_HELLSCREAM, "grom_hellscream", "Grom Hellscream"),
   UNIT(HUMAN_TANKER, "human_tanker", "Human Tanker"),
   UNIT(ORC_TANKER, "orc_tanker", "Orc Tanker"),
   UNIT(HUMAN_TRANSPORT, "human_transport", "Human Transport"),
   UNIT(ORC_TRANSPORT, "orc_transport", "Orc Transport"),
   UNIT(ELVEN_DESTROYER, "elven_destroyer", "Elven Destroyer"),
   UNIT(TROLL_DESTROYER, "troll_destroyer", "Troll Destroyer"),
   UNIT(BATTLESHIP, "battleship", "Battleship"),
   UNIT(JUGGERNAUGHT, "juggernaught", "Juggernaught"),
   HERO(DEATHWING, "deathwing", "Deathwing"),
   UNIT(GNOMISH_SUBMARINE, "gnomish_submarine", "Gnomish Submarine"),
   UNIT(GIANT_TURTLE, "giant_turtle", "Giant Turtle"),
   UNIT(GNOMISH_FLYING_MACHINE, "gnomish_flying_machine", "Gnomish Flying Machine"),
   UNIT(GOBLIN_ZEPPLIN, "goblin_zepplin", "Goblin Zepplin"),
   UNIT(GRYPHON_RIDER, "gryphon_rider", "Gryphon Rider"),
   UNIT(DRAGON, "dragon", "Dragon"),
   HERO(TURALYON, "turalyon", "Turalyon"),
   UNIT(EYE_OF_KILROGG, "eye_of_kilrogg", "Eye of Kilrogg"),
   HERO(DANATH, "danath", "Danath"),
   HERO(KARGATH_BLADEFIST, "kargath_bladefist", "Kargath Bladefist"),
   HERO(CHO_GALL, "cho_gall", "Cho'gall"),
   HERO(LOTHAR, "lothar", "Lothar"),
   HERO(GUL_DAN, "gul_dan", "Gul'dan"),
   HERO(UTHER_LIGHTBRINGER, "uther_lightbringer", "Uther Lightbringer"),
   HERO(ZUL_JIN, "zul_jin", "Zul'jin"),
   UNIT(SKELETON, "skeleton", "Skeleton"),
   UNIT(DAEMON, "daemon", "Daemon"),
   UNIT(CRITTER, "critter", "Critter"),
   UNIT(FARM, "farm", "Farm"),
   UNIT(PIG_FARM, "pig_farm", "Pig Farm"),
   UNIT(HUMAN_BARRACKS, "human_barracks", "Human Barracks"),
   UNIT(ORC_BARRACKS, "orc_barracks", "Orc Barracks"),
   UNIT(CHURCH, "church", "Church"),
   UNIT(ALTAR_OF_STORMS, "altar_of_storms", "Altar of Storms"),
   UNIT(HUMAN_SCOUT_TOWER, "human_scout_tower", "Human Scout_Tower"),
   UNIT(ORC_SCOUT_TOWER, "orc_scout_tower", "Orc Scout Tower"),
   UNIT(STABLES, "stables", "Stables"),
   UNIT(OGRE_MOUND, "ogre_mound", "Ogre Mound"),
   UNIT(GNOMISH_INVENTOR, "gnomish_inventor", "Gnomish Inventor"),
   UNIT(GOBLIN_ALCHEMIST, "goblin_alchemist", "Goblin Alchemist"),
   UNIT(GRYPHON_AVIARY, "gryphon_aviary", "Gryphon Aviary"),
   UNIT(DRAGON_ROOST, "dragon_roost", "Dragon Roost"),
   UNIT(HUMAN_SHIPYARD, "human_shipyard", "Human Shipyard"),
   UNIT(ORC_SHIPYARD, "orc_shipyard", "Orc Shipyard"),
   UNIT(TOWN_HALL, "town_hall", "Town Hall"),
   UNIT(GREAT_HALL, "great_hall", "Great Hall"),
   UNIT(ELVEN_LUMBER_MILL, "elven_lumber_mill", "Elven Lumber Mill"),
   UNIT(TROLL_LUMBER_MILL, "troll_lumber_mill", "Troll Lumber Mill"),
   UNIT(HUMAN_FOUNDRY, "human_foundry", "Human Foundry"),
   UNIT(ORC_FOUNDRY, "orc_foundry", "Orc Foundry"),
   UNIT(MAGE_TOWER, "mage_tower", "Mage Tower"),
   UNIT(TEMPLE_OF_THE_DAMNED, "temple_of_the_damned", "Temple of the Damned"),
   UNIT(HUMAN_BLACKSMITH, "human_blacksmith", "Human Blacksmith"),
   UNIT(ORC_BLACKSMITH, "orc_blacksmith", "Orc Blacksmith"),
   UNIT(HUMAN_REFINERY, "human_refinery", "Human Refinery"),
   UNIT(ORC_REFINERY, "orc_refinery", "Orc Refinery"),
   UNIT(HUMAN_OIL_WELL, "human_oil_well", "Human Oil Well"),
   UNIT(ORC_OIL_WELL, "orc_oil_well", "Orc Oil Well"),
   UNIT(KEEP, "keep", "Keep"),
   UNIT(STRONGHOLD, "stronghold", "Stronghold"),
   UNIT(CASTLE, "castle", "Castle"),
   UNIT(FORTRESS, "fortress", "Fortress"),
   UNIT(GOLD_MINE, "gold_mine", "Gold Mine"),
   UNIT(OIL_PATCH, "oil_patch", "Oil Patch"),
   UNIT_START_LOCATION(HUMAN_START, "human_start", "Human Start Location"),
   UNIT_START_LOCATION(ORC_START, "orc_start", "Orc Start Location"),
   UNIT(HUMAN_GUARD_TOWER, "human_guard_tower", "Human Guard Tower"),
   UNIT(ORC_GUARD_TOWER, "orc_guard_tower", "Orc Guard Tower"),
   UNIT(HUMAN_CANNON_TOWER, "human_cannon_tower", "Human Cannon Tower"),
   UNIT(ORC_CANNON_TOWER, "orc_cannon_tower", "Orc Cannon Tower"),
   UNIT(CIRCLE_OF_POWER, "circle_of_power", "Circle of Power"),
   UNIT(DARK_PORTAL, "dark_portal", "Dark Portal"),
   UNIT(RUNESTONE, "runestone", "Runestone"),
   UNIT(HUMAN_WALL, "human_wall", "Human Wall"),
   UNIT(ORC_WALL, "orc_wall", "Orc Wall"),
   UNIT(CRITTER_SHEEP, "sheep", "Sheep"),
   UNIT(CRITTER_PIG, "pig", "Pig"),
   UNIT(CRITTER_SEAL, "seal", "Seal"),
   UNIT(CRITTER_RED_PIG, "red_pig", "Red Pig"),
};

const char *
pud_unit2str(Pud_Unit unit,
             Pud_Bool pretty)
{
   if ((unit < PUD_UNIT_FOOTMAN) && (unit > PUD_UNIT_CRITTER_RED_PIG))
     return NULL;
   return (pretty) ? _names[unit].pretty : _names[unit].name;
}

Pud_Icon
pud_unit_icon_get(Pud_Unit unit)
{
   if ((unit < PUD_UNIT_FOOTMAN) && (unit > PUD_UNIT_CRITTER_RED_PIG))
     return PUD_ICON_CANCEL;
   return _names[unit].icon;
}



static const char *_projectiles[0x1d + 1] =
{
   "Lightning",
   "Griffon Hammer",
   "Dragon Breath",
   "Flame Shield",
   "Flame Shield (Self)",
   "Blizzard",
   "Death and Decay",
   "Big Cannon",
   "Black Powder",
   "Heal Effect",
   "Touch of Death",
   "Rune",
   "Tornado",
   "Catapult Rock",
   "Ballista Bolt",
   "Arrow",
   "Axe",
   "Submarine Missile",
   "Turtle Missile",
   "Dark Flame",
   "Bright Flame",
   "Blood",
   "More Black Powder",
   "Explosion",
   "Small Cannon",
   "Metal Spark",
   "Mini Explosion",
   "Daemon Fire",
   "Green Cross",
   "None",
};

const char *
pud_projectile2str(Pud_Projectile proj)
{
   if ((unsigned)proj > 0x1d) return NULL;
   return _projectiles[proj];
}

Pud_Bool
pud_unit_hero_is(Pud_Unit unit)
{
   return _names[unit].hero;
}
