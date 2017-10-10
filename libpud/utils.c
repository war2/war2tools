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

PUDAPI Pud_Owner
pud_owner_convert(uint8_t code)
{
   if (code == 0x00 || code == 0x02 || (code >= 0x08 && code <= 0xff))
      return PUD_OWNER_PASSIVE_COMPUTER;
   else if (code == 0x01 || code == 0x04)
      return PUD_OWNER_COMPUTER;
   else if (code == 0x05)
      return PUD_OWNER_HUMAN;
   else if (code == 0x06)
      return PUD_OWNER_RESCUE_PASSIVE;
   else if (code == 0x07)
      return PUD_OWNER_RESCUE_ACTIVE;
   else
      return PUD_OWNER_NOBODY;
}

PUDAPI Pud_Side
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

PUDAPI void
pud_dimensions_to_size(Pud_Dimensions  dim,
                       unsigned int   *w_ret,
                       unsigned int   *h_ret)
{
   int w, h;

   switch (dim)
     {
      case PUD_DIMENSIONS_32_32:
         w = 32; h = 32;
         break;

      case PUD_DIMENSIONS_64_64:
         w = 64; h = 64;
         break;

      case PUD_DIMENSIONS_96_96:
         w = 96; h = 96;
         break;

      case PUD_DIMENSIONS_128_128:
         w = 128; h = 128;
         break;

      default:
         w = 0; h = 0;
         break;
     }

   if (w_ret) *w_ret = w;
   if (h_ret) *h_ret = h;
}

PUDAPI const char *
pud_dimensions_to_string(Pud_Dimensions dims)
{
   const char *const strings[] = {
      [PUD_DIMENSIONS_UNDEFINED] = "UNDEFINED",
      [PUD_DIMENSIONS_32_32] = "32 x 32",
      [PUD_DIMENSIONS_64_64] = "64 x 64",
      [PUD_DIMENSIONS_96_96] = "96 x 96",
      [PUD_DIMENSIONS_128_128] = "128 x 128",
   };
   return ((unsigned)dims > PUD_DIMENSIONS_128_128) ? NULL : strings[dims];
}

PUDAPI const char *
pud_color_to_string(Pud_Player color)
{
   const char *const colors[] = {
      "Red", "Blue", "Green", "Violet",
      "Orange", "Black", "White", "Yellow"
   };

   return ((unsigned)color > PUD_PLAYER_YELLOW) ? NULL : colors[color];
}

PUDAPI const char *
pud_era_to_string(Pud_Era era)
{
   const char *const eras[] = {
      "Forest", "Winter", "Wasteland", "Swamp"
   };

   return ((unsigned)era > PUD_ERA_SWAMP) ? NULL : eras[era];
}

typedef struct
{
   const char *const name;
   const char *const pretty;
   const Pud_Icon icon;
   const Pud_Bool hero;
   const unsigned int size;
} Unit;

#define UNIT(suffix_, name_, pretty_, size_) \
   [PUD_UNIT_ ## suffix_] = { \
      .name = name_, \
      .pretty = pretty_, \
      .icon = PUD_ICON_ ## suffix_, \
      .hero = PUD_FALSE, \
      .size = size_, \
   }

#define HERO(suffix_, name_, pretty_, size_) \
   [PUD_UNIT_ ## suffix_] = { \
      .name = name_, \
      .pretty = pretty_, \
      .icon = PUD_ICON_ ## suffix_, \
      .hero = PUD_TRUE, \
      .size = size_, \
   }

#define UNIT_START_LOCATION(suffix_, name_, pretty_) \
   [PUD_UNIT_ ## suffix_] = { \
      .name = name_, \
      .pretty = pretty_, \
      .icon = PUD_ICON_CANCEL, \
      .hero = PUD_FALSE, \
      .size = 1, \
   }

#define UNITx1(suffix_, name_, pretty_) UNIT(suffix_, name_, pretty_, 1)
#define UNITx2(suffix_, name_, pretty_) UNIT(suffix_, name_, pretty_, 2)
#define UNITx3(suffix_, name_, pretty_) UNIT(suffix_, name_, pretty_, 3)
#define UNITx4(suffix_, name_, pretty_) UNIT(suffix_, name_, pretty_, 4)

#define HEROx1(suffix_, name_, pretty_) HERO(suffix_, name_, pretty_, 1)
#define HEROx2(suffix_, name_, pretty_) HERO(suffix_, name_, pretty_, 2)
#define HEROx3(suffix_, name_, pretty_) HERO(suffix_, name_, pretty_, 3)
#define HEROx4(suffix_, name_, pretty_) HERO(suffix_, name_, pretty_, 4)


static const Unit _names[110] =
{
   UNITx1(FOOTMAN, "footman", "Footman"),
   UNITx1(GRUNT, "grunt", "Grunt"),
   UNITx1(PEASANT, "peasant", "Peasant"),
   UNITx1(PEON, "peon", "Peon"),
   UNITx1(BALLISTA, "ballista", "Ballista"),
   UNITx1(CATAPULT, "catapult", "Catapult"),
   UNITx1(KNIGHT, "knight", "Knight"),
   UNITx1(OGRE, "ogre", "Ogre"),
   UNITx1(ARCHER, "archer", "Archer"),
   UNITx1(AXETHROWER, "axethrower", "Axethrower"),
   UNITx1(MAGE, "mage", "Mage"),
   UNITx1(DEATH_KNIGHT, "death_knight", "Death Knight"),
   UNITx1(PALADIN, "paladin", "Paladin"),
   UNITx1(OGRE_MAGE, "ogre_mage", "Ogre Mage"),
   UNITx1(DWARVES, "dwarves", "Dwarven Demolition Squad"),
   UNITx1(GOBLIN_SAPPER, "goblin_sapper", "Goblin Sapper"),
   UNITx1(ATTACK_PEASANT, "attack_peasant", "Peasant (Attack)"),
   UNITx1(ATTACK_PEON, "attack_peon", "Peon (Attack)"),
   UNITx1(RANGER, "ranger", "Elven Ranger"),
   UNITx1(BERSERKER, "berserker", "Berserker"),
   HEROx1(ALLERIA, "alleria", "Alleria"),
   HEROx1(TERON_GOREFIEND, "teron_gorefiend", "Teron Gorefiend"),
   HEROx2(KURDRAN_AND_SKY_REE, "kurdran_and_sky_ree", "Kurdran and Sky'ree"),
   HEROx1(DENTARG, "dentarg", "Dentarg"),
   HEROx1(KHADGAR, "khadgar", "Khadgar"),
   HEROx1(GROM_HELLSCREAM, "grom_hellscream", "Grom Hellscream"),
   UNITx2(HUMAN_TANKER, "human_tanker", "Human Tanker"),
   UNITx2(ORC_TANKER, "orc_tanker", "Orc Tanker"),
   UNITx2(HUMAN_TRANSPORT, "human_transport", "Human Transport"),
   UNITx2(ORC_TRANSPORT, "orc_transport", "Orc Transport"),
   UNITx2(ELVEN_DESTROYER, "elven_destroyer", "Elven Destroyer"),
   UNITx2(TROLL_DESTROYER, "troll_destroyer", "Troll Destroyer"),
   UNITx2(BATTLESHIP, "battleship", "Battleship"),
   UNITx2(JUGGERNAUGHT, "juggernaught", "Juggernaught"),
   HEROx3(DEATHWING, "deathwing", "Deathwing"),
   UNITx2(GNOMISH_SUBMARINE, "gnomish_submarine", "Gnomish Submarine"),
   UNITx2(GIANT_TURTLE, "giant_turtle", "Giant Turtle"),
   UNITx2(GNOMISH_FLYING_MACHINE, "gnomish_flying_machine", "Gnomish Flying Machine"),
   UNITx2(GOBLIN_ZEPPLIN, "goblin_zepplin", "Goblin Zepplin"),
   UNITx2(GRYPHON_RIDER, "gryphon_rider", "Gryphon Rider"),
   UNITx3(DRAGON, "dragon", "Dragon"),
   HEROx1(TURALYON, "turalyon", "Turalyon"),
   UNITx1(EYE_OF_KILROGG, "eye_of_kilrogg", "Eye of Kilrogg"),
   HEROx1(DANATH, "danath", "Danath"),
   HEROx1(KARGATH_BLADEFIST, "kargath_bladefist", "Kargath Bladefist"),
   HEROx1(CHO_GALL, "cho_gall", "Cho'gall"),
   HEROx1(LOTHAR, "lothar", "Lothar"),
   HEROx1(GUL_DAN, "gul_dan", "Gul'dan"),
   HEROx1(UTHER_LIGHTBRINGER, "uther_lightbringer", "Uther Lightbringer"),
   HEROx1(ZUL_JIN, "zul_jin", "Zul'jin"),
   UNITx1(SKELETON, "skeleton", "Skeleton"),
   UNITx1(DAEMON, "daemon", "Daemon"),
   UNITx1(CRITTER, "critter", "Critter"),
   UNITx2(FARM, "farm", "Farm"),
   UNITx2(PIG_FARM, "pig_farm", "Pig Farm"),
   UNITx3(HUMAN_BARRACKS, "human_barracks", "Human Barracks"),
   UNITx3(ORC_BARRACKS, "orc_barracks", "Orc Barracks"),
   UNITx3(CHURCH, "church", "Church"),
   UNITx3(ALTAR_OF_STORMS, "altar_of_storms", "Altar of Storms"),
   UNITx1(HUMAN_SCOUT_TOWER, "human_scout_tower", "Human Scout_Tower"),
   UNITx1(ORC_SCOUT_TOWER, "orc_scout_tower", "Orc Scout Tower"),
   UNITx3(STABLES, "stables", "Stables"),
   UNITx3(OGRE_MOUND, "ogre_mound", "Ogre Mound"),
   UNITx3(GNOMISH_INVENTOR, "gnomish_inventor", "Gnomish Inventor"),
   UNITx3(GOBLIN_ALCHEMIST, "goblin_alchemist", "Goblin Alchemist"),
   UNITx3(GRYPHON_AVIARY, "gryphon_aviary", "Gryphon Aviary"),
   UNITx3(DRAGON_ROOST, "dragon_roost", "Dragon Roost"),
   UNITx3(HUMAN_SHIPYARD, "human_shipyard", "Human Shipyard"),
   UNITx3(ORC_SHIPYARD, "orc_shipyard", "Orc Shipyard"),
   UNITx4(TOWN_HALL, "town_hall", "Town Hall"),
   UNITx4(GREAT_HALL, "great_hall", "Great Hall"),
   UNITx3(ELVEN_LUMBER_MILL, "elven_lumber_mill", "Elven Lumber Mill"),
   UNITx3(TROLL_LUMBER_MILL, "troll_lumber_mill", "Troll Lumber Mill"),
   UNITx3(HUMAN_FOUNDRY, "human_foundry", "Human Foundry"),
   UNITx3(ORC_FOUNDRY, "orc_foundry", "Orc Foundry"),
   UNITx3(MAGE_TOWER, "mage_tower", "Mage Tower"),
   UNITx3(TEMPLE_OF_THE_DAMNED, "temple_of_the_damned", "Temple of the Damned"),
   UNITx3(HUMAN_BLACKSMITH, "human_blacksmith", "Human Blacksmith"),
   UNITx3(ORC_BLACKSMITH, "orc_blacksmith", "Orc Blacksmith"),
   UNITx3(HUMAN_REFINERY, "human_refinery", "Human Refinery"),
   UNITx3(ORC_REFINERY, "orc_refinery", "Orc Refinery"),
   UNITx3(HUMAN_OIL_WELL, "human_oil_well", "Human Oil Well"),
   UNITx3(ORC_OIL_WELL, "orc_oil_well", "Orc Oil Well"),
   UNITx4(KEEP, "keep", "Keep"),
   UNITx4(STRONGHOLD, "stronghold", "Stronghold"),
   UNITx4(CASTLE, "castle", "Castle"),
   UNITx4(FORTRESS, "fortress", "Fortress"),
   UNITx3(GOLD_MINE, "gold_mine", "Gold Mine"),
   UNITx3(OIL_PATCH, "oil_patch", "Oil Patch"),
   UNIT_START_LOCATION(HUMAN_START, "human_start", "Human Start Location"),
   UNIT_START_LOCATION(ORC_START, "orc_start", "Orc Start Location"),
   UNITx2(HUMAN_GUARD_TOWER, "human_guard_tower", "Human Guard Tower"),
   UNITx2(ORC_GUARD_TOWER, "orc_guard_tower", "Orc Guard Tower"),
   UNITx2(HUMAN_CANNON_TOWER, "human_cannon_tower", "Human Cannon Tower"),
   UNITx2(ORC_CANNON_TOWER, "orc_cannon_tower", "Orc Cannon Tower"),
   UNITx2(CIRCLE_OF_POWER, "circle_of_power", "Circle of Power"),
   UNITx4(DARK_PORTAL, "dark_portal", "Dark Portal"),
   UNITx2(RUNESTONE, "runestone", "Runestone"),
   UNITx1(HUMAN_WALL, "human_wall", "Human Wall"),
   UNITx1(ORC_WALL, "orc_wall", "Orc Wall"),
   UNITx1(CRITTER_SHEEP, "sheep", "Sheep"),
   UNITx1(CRITTER_PIG, "pig", "Pig"),
   UNITx1(CRITTER_SEAL, "seal", "Seal"),
   UNITx1(CRITTER_RED_PIG, "red_pig", "Red Pig"),
};

PUDAPI const char *
pud_unit_to_string(Pud_Unit unit,
             Pud_Bool pretty)
{
   if ((unit < PUD_UNIT_FOOTMAN) || (unit > PUD_UNIT_CRITTER_RED_PIG))
     return NULL;
   return (pretty) ? _names[unit].pretty : _names[unit].name;
}

PUDAPI Pud_Icon
pud_unit_icon_get(Pud_Unit unit)
{
   if ((unit < PUD_UNIT_FOOTMAN) || (unit > PUD_UNIT_CRITTER_RED_PIG))
     return PUD_ICON_CANCEL;
   return _names[unit].icon;
}

PUDAPI unsigned int
pud_unit_size_get(Pud_Unit unit)
{
   if ((unit < PUD_UNIT_FOOTMAN) || (unit > PUD_UNIT_CRITTER_RED_PIG))
     return 0;
   return _names[unit].size;
}

static const char *const _projectiles[0x1d + 1] =
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

PUDAPI const char *
pud_projectile_to_string(Pud_Projectile proj)
{
   if ((unsigned)proj > 0x1d) return NULL;
   return _projectiles[proj];
}

PUDAPI Pud_Bool
pud_unit_hero_is(Pud_Unit unit)
{
   return _names[unit].hero;
}


typedef struct
{
   const char *name;
   Pud_Icon icon;
} upgrades;

#define UGRD(ugrd_, name_) \
   [PUD_UPGRADE_ ## ugrd_] = { \
      .name = name_, \
      .icon = PUD_ICON_ ## ugrd_, \
   }

static const upgrades _upgrades[52] =
{
   UGRD(SWORD_1, "Human Swords (+1)"),
   UGRD(SWORD_2, "Human Swords (+2)"),
   UGRD(AXE_1, "Orc Axes (+1)"),
   UGRD(AXE_2, "Orc Axes (+2)"),
   UGRD(ARROW_1, "Human Arrows (+1)"),
   UGRD(ARROW_2, "Human Arrows (+2)"),
   UGRD(SPEAR_1, "Orc Spears (+1)"),
   UGRD(SPEAR_2, "Orc Spears (+2)"),
   UGRD(HUMAN_SHIELD_1, "Human Shields (+1)"),
   UGRD(HUMAN_SHIELD_2, "Human Shields (+2)"),
   UGRD(ORC_SHIELD_1, "Orc Shields (+1)"),
   UGRD(ORC_SHIELD_2, "Orc Shields (+2)"),
   UGRD(HUMAN_SHIP_CANNON_1, "Human Ship Cannons (+1)"),
   UGRD(HUMAN_SHIP_CANNON_2, "Human Ship Cannons (+2)"),
   UGRD(ORC_SHIP_CANNON_1, "Orc Ship Cannons (+1)"),
   UGRD(ORC_SHIP_CANNON_2, "Orc Ship Cannons (+2)"),
   UGRD(HUMAN_SHIP_ARMOR_1, "Human Ship Armors (+1)"),
   UGRD(HUMAN_SHIP_ARMOR_2, "Human Ship Armors (+2)"),
   UGRD(ORC_SHIP_ARMOR_1, "Orc Ship Armors (+1)"),
   UGRD(ORC_SHIP_ARMOR_2, "Orc Ship Armors (+2)"),
   UGRD(CATAPULT_1, "Catapult Projectiles (+1)"),
   UGRD(CATAPULT_2, "Catapult Projectiles (+2)"),
   UGRD(BALLISTA_1, "Ballista Projectiles (+1)"),
   UGRD(BALLISTA_2, "Ballista Projectiles (+2)"),
   UGRD(TRAIN_RANGERS, "Train Rangers"),
   UGRD(LONGBOW, "Long Bow"),
   UGRD(RANGER_SCOUTING, "Elven Scouting"),
   UGRD(RANGER_MARKSMANSHIP, "Marksmanship"),
   UGRD(TRAIN_BERSERKERS, "Train Berserkers"),
   UGRD(LIGHTER_AXES, "Lighter Axes"),
   UGRD(BERSERKER_SCOUTING, "Berserker Scouting"),
   UGRD(BERSERKER_REGENERATION, "Berserker Regeneration"),
   UGRD(TRAIN_OGRE_MAGES, "Train Ogre Mages"),
   UGRD(TRAIN_PALADINS, "Train Paladins"),
   UGRD(HOLY_VISION, "Holy Vision"),
   UGRD(HEALING, "Healing"),
   UGRD(EXORCISM, "Exorcism"),
   UGRD(FLAME_SHIELD, "Flame Shield"),
   UGRD(FIREBALL, "Fireball"),
   UGRD(SLOW, "Slow"),
   UGRD(INVISIBILITY, "Invisibility"),
   UGRD(POLYMORPH, "Polymorph"),
   UGRD(BLIZZARD, "Blizzard"),
   UGRD(EYE_OF_KILROGG, "Eye of Kilrogg"),
   UGRD(BLOODLUST, "Bloodlust"),
   UGRD(RAISE_DEAD, "Raise Dead"),
   UGRD(DEATH_COIL, "Death Coil"),
   UGRD(WHIRLWIND, "Whirlwind"),
   UGRD(HASTE, "Haste"),
   UGRD(UNHOLY_ARMOR, "Unholy Armor"),
   UGRD(RUNES, "Runes"),
   UGRD(DEATH_AND_DECAY, "Death and Decay"),
};

PUDAPI Pud_Icon
pud_upgrade_icon_get(Pud_Upgrade upgrades)
{
   return ((unsigned)upgrades < 52) ? _upgrades[upgrades].icon : PUD_ICON_CANCEL;
}

PUDAPI const char *
pud_upgrade_to_string(Pud_Upgrade upgrades)
{
   return ((unsigned)upgrades < 52) ? _upgrades[upgrades].name : NULL;
}

PUDAPI Pud_Bool
pud_allow_unit_valid_is(Pud_Allow flag)
{
   /*
    * Flag must be non zero, bits 13 and 31 must not be set
    */
   return ((flag) &&
           (!(flag & (1 << 13))) &&
           (!(flag & (1 << 31))));
}

PUDAPI Pud_Bool
pud_allow_spell_valid_is(Pud_Allow flag)
{
   /*
    * Flag must be non zero, bits 2, 12 and from 20 to 31 must not be set
    */
   return ((flag) &&
           (!(flag & 0xfff00000)) &&
           (!(flag & (1 << 2))) &&
           (!(flag & (1 << 12))));
}

PUDAPI Pud_Bool
pud_allow_upgrade_valid_is(Pud_Allow flag)
{
   /*
    * Flag must be non zero, bits 10, 11, 14, 15 and from 20 to 31 must not be set
    */
   return ((flag) &&
           (!(flag & 0xfff00000)) &&
           (!(flag & (1 << 10))) &&
           (!(flag & (1 << 11))) &&
           (!(flag & (1 << 14))) &&
           (!(flag & (1 << 15))));
}

typedef struct
{
   const char *name;
   Pud_Icon icons[2];
} Alow_Unit;

#define ALOW_UNIT(name_, icon1_, icon2_) \
   { \
      .name = name_, \
      .icons = { \
         icon1_, \
         icon2_ \
      } \
   }

#define ALOW_UNIT_UNUSED() \
   { .name = NULL, .icons = { PUD_ICON_CANCEL, PUD_ICON_CANCEL } }

static const Alow_Unit _alow_units[] =
{
   ALOW_UNIT("Footman / Grunt", PUD_ICON_FOOTMAN, PUD_ICON_GRUNT),
   ALOW_UNIT("Peasant / Peon", PUD_ICON_PEASANT, PUD_ICON_PEON),
   ALOW_UNIT("Ballista / Catapult", PUD_ICON_BALLISTA, PUD_ICON_CATAPULT),
   ALOW_UNIT("Knight / Ogre", PUD_ICON_KNIGHT, PUD_ICON_OGRE),
   ALOW_UNIT("Archer / Axethrower", PUD_ICON_ARCHER, PUD_ICON_AXETHROWER),
   ALOW_UNIT("Mage / Death Knight", PUD_ICON_MAGE, PUD_ICON_DEATH_KNIGHT),
   ALOW_UNIT("Human Tanker / Orc Tanker", PUD_ICON_HUMAN_TANKER, PUD_ICON_ORC_TANKER),
   ALOW_UNIT("Elven Destroyer / Troll Destroyer", PUD_ICON_ELVEN_DESTROYER, PUD_ICON_TROLL_DESTROYER),
   ALOW_UNIT("Human Transport / Orc Transport", PUD_ICON_HUMAN_TRANSPORT, PUD_ICON_ORC_TRANSPORT),
   ALOW_UNIT("Battleship / Juggernaught", PUD_ICON_BATTLESHIP, PUD_ICON_JUGGERNAUGHT),
   ALOW_UNIT("Gnomish Submarine / Giant Turtle", PUD_ICON_GNOMISH_SUBMARINE, PUD_ICON_GIANT_TURTLE),
   ALOW_UNIT("Gnomish Flying Machine / Goblin Zepplin", PUD_ICON_GNOMISH_FLYING_MACHINE, PUD_ICON_GOBLIN_ZEPPLIN),
   ALOW_UNIT("Gryphon Rider / Dragon", PUD_ICON_GRYPHON_RIDER, PUD_ICON_DRAGON),
   ALOW_UNIT_UNUSED(),
   ALOW_UNIT("Dwarven Demolition Squad / Goblin Sapper", PUD_ICON_DWARVES, PUD_ICON_GOBLIN_SAPPER),
   ALOW_UNIT("Gryphon Aviary / Dragon Roost", PUD_ICON_GRYPHON_AVIARY, PUD_ICON_DRAGON_ROOST),
   ALOW_UNIT("Farm / Pig Farm", PUD_ICON_FARM, PUD_ICON_PIG_FARM),
   ALOW_UNIT("Human Barracks / Orc Barracks", PUD_ICON_HUMAN_BARRACKS, PUD_ICON_ORC_BARRACKS),
   ALOW_UNIT("Elven Lumber Mill / Troll Lumber Mill", PUD_ICON_ELVEN_LUMBER_MILL, PUD_ICON_TROLL_LUMBER_MILL),
   ALOW_UNIT("Stables / Ogre Mound", PUD_ICON_STABLES, PUD_ICON_OGRE_MOUND),
   ALOW_UNIT("Mage Tower / Temple of the Damned", PUD_ICON_MAGE_TOWER, PUD_ICON_TEMPLE_OF_THE_DAMNED),
   ALOW_UNIT("Human Foundry / Orc Foundry", PUD_ICON_HUMAN_FOUNDRY, PUD_ICON_ORC_FOUNDRY),
   ALOW_UNIT("Human Refinery / Orc Refinery", PUD_ICON_HUMAN_REFINERY, PUD_ICON_ORC_REFINERY),
   ALOW_UNIT("Gnomish Inventor / Goblin Alchemist", PUD_ICON_GNOMISH_INVENTOR, PUD_ICON_GOBLIN_ALCHEMIST),
   ALOW_UNIT("Church / Altar of Storms", PUD_ICON_CHURCH, PUD_ICON_ALTAR_OF_STORMS),
   ALOW_UNIT("Human Scout Tower / Orc Scout Tower", PUD_ICON_HUMAN_SCOUT_TOWER, PUD_ICON_ORC_SCOUT_TOWER),
   ALOW_UNIT("Town Hall / Great Hall", PUD_ICON_TOWN_HALL, PUD_ICON_GREAT_HALL),
   ALOW_UNIT("Keep / Stronghold", PUD_ICON_KEEP, PUD_ICON_STRONGHOLD),
   ALOW_UNIT("Castle / Fortress", PUD_ICON_CASTLE, PUD_ICON_FORTRESS),
   ALOW_UNIT("Human Blacksmith / Orc Blashsmith", PUD_ICON_HUMAN_BLACKSMITH, PUD_ICON_ORC_BLACKSMITH),
   ALOW_UNIT("Human Shipyard / Orc Shipyard", PUD_ICON_HUMAN_SHIPYARD, PUD_ICON_ORC_SHIPYARD),
   ALOW_UNIT_UNUSED()
};

typedef struct
{
   const char *name;
   Pud_Icon icon;
} Alow_Spell;

#define ALOW_SPELL(name_, icon_) \
   { \
      .name = name_, \
      .icon = icon_, \
   }

#define ALOW_SPELL_UNUSED() \
   { .name = NULL, .icon = PUD_ICON_CANCEL }

static const Alow_Spell _alow_spells[] =
{
   ALOW_SPELL("Holy Vision", PUD_ICON_HOLY_VISION),
   ALOW_SPELL("Healing", PUD_ICON_HEALING),
   ALOW_SPELL_UNUSED(),
   ALOW_SPELL("Exorcism", PUD_ICON_EXORCISM),
   ALOW_SPELL("Flame Shield", PUD_ICON_FLAME_SHIELD),
   ALOW_SPELL("Fireball",  PUD_ICON_FIREBALL),
   ALOW_SPELL("Slow",  PUD_ICON_SLOW),
   ALOW_SPELL("Invisibility",  PUD_ICON_INVISIBILITY),
   ALOW_SPELL("Polymorph",  PUD_ICON_POLYMORPH),
   ALOW_SPELL("Blizzard",  PUD_ICON_BLIZZARD),
   ALOW_SPELL("Eye of Kilrogg",  PUD_ICON_EYE_OF_KILROGG),
   ALOW_SPELL("Bloodlust",  PUD_ICON_BLOODLUST),
   ALOW_SPELL_UNUSED(),
   ALOW_SPELL("Raise Dead",  PUD_ICON_RAISE_DEAD),
   ALOW_SPELL("Death Coil",  PUD_ICON_DEATH_COIL),
   ALOW_SPELL("Whirlwind",  PUD_ICON_WHIRLWIND),
   ALOW_SPELL("Haste",  PUD_ICON_HASTE),
   ALOW_SPELL("Unholy Armor",  PUD_ICON_UNHOLY_ARMOR),
   ALOW_SPELL("Runes",  PUD_ICON_RUNES),
   ALOW_SPELL("Death and Decay",  PUD_ICON_DEATH_AND_DECAY),
};

/* Upgrades are handle the same way than units */
typedef Alow_Unit Alow_Upgrade;
#define ALOW_UGRD(...) ALOW_UNIT(__VA_ARGS__)
#define ALOW_UGRD_UNUSED() ALOW_UNIT_UNUSED()

static const Alow_Upgrade _alow_upgrades[] =
{
   ALOW_UGRD("Human Arrows / Orc Spears (+1)", PUD_ICON_ARROW_1, PUD_ICON_SPEAR_1),
   ALOW_UGRD("Human Arrows / Orc Spears (+2)", PUD_ICON_ARROW_2, PUD_ICON_SPEAR_2),
   ALOW_UGRD("Human Swords / Orc Axes (+1)", PUD_ICON_SWORD_1, PUD_ICON_AXE_1),
   ALOW_UGRD("Human Swords / Orc Axes (+2)", PUD_ICON_SWORD_2, PUD_ICON_AXE_2),
   ALOW_UGRD("Human Shields / Orc Shields (+1)", PUD_ICON_HUMAN_SHIELD_1, PUD_ICON_ORC_SHIELD_1),
   ALOW_UGRD("Human Shields / Orc Shields (+2)", PUD_ICON_HUMAN_SHIELD_2, PUD_ICON_ORC_SHIELD_2),
   ALOW_UGRD("Human Ship Cannons / Orc Ship Cannons (+1)", PUD_ICON_HUMAN_SHIP_CANNON_1, PUD_ICON_ORC_SHIP_CANNON_1),
   ALOW_UGRD("Human Ship Cannons / Orc Ship Cannons (+2)", PUD_ICON_HUMAN_SHIP_CANNON_2, PUD_ICON_ORC_SHIP_CANNON_2),
   ALOW_UGRD("Human Ship Armor / Orc Ship Armor (+1)", PUD_ICON_HUMAN_SHIP_ARMOR_1, PUD_ICON_ORC_SHIP_ARMOR_1),
   ALOW_UGRD("Human Ship Armor / Orc Ship Armor (+2)", PUD_ICON_HUMAN_SHIP_ARMOR_2, PUD_ICON_ORC_SHIP_ARMOR_2),
   ALOW_UNIT_UNUSED(),
   ALOW_UNIT_UNUSED(),
   ALOW_UGRD("Ballista Projectiles / Catapult Projectiles (+1)", PUD_ICON_BALLISTA_1, PUD_ICON_CATAPULT_1),
   ALOW_UGRD("Ballista Projectiles / Catapult Projectiles (+2)", PUD_ICON_BALLISTA_2, PUD_ICON_CATAPULT_2),
   ALOW_UNIT_UNUSED(),
   ALOW_UNIT_UNUSED(),
   ALOW_UGRD("Rangers / Berserkers", PUD_ICON_TRAIN_RANGERS, PUD_ICON_TRAIN_BERSERKERS),
   ALOW_UGRD("Long Bow / Lighter Axes", PUD_ICON_LONGBOW, PUD_ICON_LIGHTER_AXES),
   ALOW_UGRD("Elven Scouting / Berserker Scouting", PUD_ICON_RANGER_SCOUTING, PUD_ICON_BERSERKER_SCOUTING),
   ALOW_UGRD("Marksmanship / Regeneration", PUD_ICON_RANGER_MARKSMANSHIP, PUD_ICON_BERSERKER_REGENERATION),
};

static unsigned int
_flag_to_index(Pud_Allow flag)
{
   unsigned int idx = 0;

   while (flag != 0x0)
     {
        idx++;
        flag >>= 1;
     }
   return (idx == 0) ? idx : idx - 1;
}

PUDAPI const char *
pud_allow_unit_to_string(Pud_Allow flag)
{
   unsigned int idx;

   idx = _flag_to_index(flag);
   return _alow_units[idx].name;
}

PUDAPI const Pud_Icon *
pud_allow_unit_icons_get(Pud_Allow flag)
{
   unsigned int idx;

   idx = _flag_to_index(flag);
   return _alow_units[idx].icons;
}

PUDAPI const char *
pud_allow_spell_to_string(Pud_Allow flag)
{
   unsigned int idx;

   idx = _flag_to_index(flag);
   return _alow_spells[idx].name;
}

PUDAPI Pud_Icon
pud_allow_spell_icon_get(Pud_Allow flag)
{
   unsigned int idx;

   idx = _flag_to_index(flag);
   return _alow_spells[idx].icon;
}

PUDAPI const char *
pud_allow_upgrade_to_string(Pud_Allow flag)
{
   unsigned int idx;

   idx = _flag_to_index(flag);
   return _alow_upgrades[idx].name;
}

PUDAPI const Pud_Icon *
pud_allow_upgrade_icons_get(Pud_Allow flag)
{
   unsigned int idx;

   idx = _flag_to_index(flag);
   return _alow_upgrades[idx].icons;
}

PUDAPI void
pud_dump(const Pud *pud,
         FILE      *stream)
{
   unsigned int i, j;

   if (!pud) DIE_RETURN(VOID, "Invalid PUD input (NULL)");
   if (!stream) stream = stdout;

   fprintf(stream, "Tag ID...............: 0x%x\n", pud->tag);
   fprintf(stream, "Version..............: %x\n", pud->version);
   fprintf(stream, "Description..........: %s\n", pud->description);
   fprintf(stream, "Era..................: %s\n", pud_era_to_string(pud->era));
   fprintf(stream, "Dimensions...........: %s\n", pud_dimensions_to_string(pud->dims));
   fprintf(stream, "Default ALOW.........: %i\n", pud->private_data->default_allow);
   fprintf(stream, "Default UDTA.........: %i\n", pud->private_data->default_udta);
   fprintf(stream, "Default UGRD.........: %i\n", pud->private_data->default_ugrd);

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
        fprintf(stream, "   upgrades 0x%02x......:\n", i);
        fprintf(stream, "      Time...........: %u\n", pud->upgrades[i].time);
        fprintf(stream, "      Gold...........: %u\n", pud->upgrades[i].gold);
        fprintf(stream, "      Lumber.........: %u\n", pud->upgrades[i].lumber);
        fprintf(stream, "      Oil............: %u\n", pud->upgrades[i].oil);
        fprintf(stream, "      Icon...........: %u\n", pud->upgrades[i].icon);
        fprintf(stream, "      Group..........: 0x%02x\n", pud->upgrades[i].group);
        fprintf(stream, "      Flags..........: %s\n", long2bin(pud->upgrades[i].flags));
     }

   /* Units */
   fprintf(stream, "Units................: %u\n", pud->units_count);
   for (i = 0; i < pud->units_count; i++)
     {
        fprintf(stream, "   Unit %04i.........:\n", i);
        fprintf(stream, "      X,Y............: %i,%i\n", pud->units[i].x, pud->units[i].y);
        fprintf(stream, "      Type...........: %s (0x%x)\n", pud_unit_to_string(pud->units[i].type, PUD_TRUE), pud->units[i].type);
        fprintf(stream, "      Player.........: 0x%x\n", pud->units[i].player);
        fprintf(stream, "      Alter..........: %u\n", pud->units[i].alter);
     }

   /* Unit data */
   fprintf(stream, "Unit Data............:\n");
   for (i = 0; i < 110; i++)
     {
        const Pud_Unit_Description *const d = &(pud->units_descr[i]);
        const char *const name = pud_unit_to_string(i, PUD_TRUE);

        if (! name) { continue; }

        fprintf(stream, "   Unit 0x%02x.........: %s\n", i, name);
        fprintf(stream, "      Overlap........: %x\n", d->overlap_frames);
        fprintf(stream, "      Sight..........: %u\n", d->sight);
        fprintf(stream, "      Hit Points.....: %u\n", d->hp);
        fprintf(stream, "      Build Time.....: %u\n", d->build_time);
        fprintf(stream, "      Gold Cost......: %u\n", d->gold_cost);
        fprintf(stream, "      Lumber Cost....: %u\n", d->lumber_cost);
        fprintf(stream, "      Oil Cost.......: %u\n", d->oil_cost);
        fprintf(stream, "      Width..........: %u\n", d->size_w);
        fprintf(stream, "      Height.........: %u\n", d->size_h);
        fprintf(stream, "      Box Width......: %u\n", d->box_w);
        fprintf(stream, "      Box Height.....: %u\n", d->box_h);
        fprintf(stream, "      Range..........: %u\n", d->range);
        fprintf(stream, "      Cptr react rg..: %u\n", d->computer_react_range);
        fprintf(stream, "      Hmn reac rg....: %u\n", d->human_react_range);
        fprintf(stream, "      Armor..........: %u\n", d->armor);
        fprintf(stream, "      Priority.......: %u\n", d->priority);
        fprintf(stream, "      Basic Dmg......: %u\n", d->basic_damage);
        fprintf(stream, "      Piercing Dmg...: %u\n", d->piercing_damage);
        fprintf(stream, "      Missile........: %u\n", d->missile_weapon);
        fprintf(stream, "      Type...........: %u\n", d->type);
        fprintf(stream, "      Decay Rate.....: %u\n", d->decay_rate);
        fprintf(stream, "      Annoy..........: %u\n", d->annoy);
        fprintf(stream, "      Mouse 2 Btn....: %u\n", d->mouse_right_btn);
        fprintf(stream, "      Point Value....: %u\n", d->point_value);
        fprintf(stream, "      Can Target.....: %u\n", d->can_target);
        fprintf(stream, "      Rect Sel.......: %i\n", d->rect_sel);
        fprintf(stream, "      Has Magic......: %i\n", d->has_magic);
        fprintf(stream, "      Weapons Ugrd...: %i\n", d->weapons_upgradable);
        fprintf(stream, "      Armor Ugrd.....: %i\n", d->armor_upgradable);
        fprintf(stream, "      Flags..........: %s\n", long2bin(d->flags));
     }

   /* Tiles map */
   fprintf(stream, "Tiles Map (%s)\n", pud_dimensions_to_string(pud->dims));
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
   fprintf(stream, "Action Map (%s)\n", pud_dimensions_to_string(pud->dims));
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
   fprintf(stream, "Movement Map (%s)\n", pud_dimensions_to_string(pud->dims));
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
