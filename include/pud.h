/**
 * @file pud.h
 * @brief Warcraft II PUDs manipulation library
 *
 * Pud provides ways to manipulate PUD files.
 * PUD files are Warcraft II custom map files, that
 * can be loaded into Warcraft II and distributed among
 * players .
 *
 * Copyright (c) 2014 -2016 Jean Guyomarc'h
 */

#ifndef _PUD_H_
#define _PUD_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @typedef Pud_Bool
 * Boolean type that can take two values among
 * #PUD_TRUE and #PUD_FALSE
 */
typedef unsigned char Pud_Bool;

/**
 * @def PUD_TRUE
 * Boolean value for TRUE (1)
 */
#define PUD_TRUE  ((Pud_Bool)(1))

/**
 * @def PUD_FALSE
 * Boolean value for FALSE (0)
 */
#define PUD_FALSE ((Pud_Bool)(0))

#define PUD_VERSION_TOD  0x0011
#define PUD_VERSION_BDP  0x0013


/**
 * @typedef Pud_Section
 * Type that represents a PUD section
 */
typedef enum
{
   PUD_SECTION_TYPE     = 0, /**< Header section */
   PUD_SECTION_VER      = 1, /**< Version section */
   PUD_SECTION_DESC     = 2, /**< Pud description section */
   PUD_SECTION_OWNR     = 3, /**< Owners section */
   PUD_SECTION_ERA      = 4, /**< Era section */
   PUD_SECTION_ERAX     = 5, /**< Extended era section */
   PUD_SECTION_DIM      = 6, /**< Dimension of the pud */
   PUD_SECTION_UDTA     = 7, /**< Units data section */
   PUD_SECTION_ALOW     = 8, /**< Allowed section */
   PUD_SECTION_UGRD     = 9, /**< Upgrades section */
   PUD_SECTION_SIDE     = 10, /**< Sides section */
   PUD_SECTION_SGLD     = 11, /**< Starting gold section */
   PUD_SECTION_SLBR     = 12, /**< Starting lumber section */
   PUD_SECTION_SOIL     = 13, /**< Starting oil section */
   PUD_SECTION_AIPL     = 14, /**< AI players section */
   PUD_SECTION_MTXM     = 15, /**< Tiles map section */
   PUD_SECTION_SQM      = 16, /**< Movement map section */
   PUD_SECTION_OILM     = 17, /**< Obsolete oil concentration map section */
   PUD_SECTION_REGM     = 18, /**< Action map section */
   PUD_SECTION_UNIT     = 19 /**< Units section */
} Pud_Section;


/**
 * @typedef Pud_Dimensions
 * Type that holds possible sizes of a PUD
 */
typedef enum
{
   PUD_DIMENSIONS_UNDEFINED = 0, /**< Undefined, invalid dimensions */
   PUD_DIMENSIONS_32_32, /**< 32x32 map */
   PUD_DIMENSIONS_64_64, /**< 64x64 map */
   PUD_DIMENSIONS_96_96, /**< 96x96 map */
   PUD_DIMENSIONS_128_128 /**< 128x128 map */
} Pud_Dimensions;

typedef enum
{
   PUD_PIXEL_FORMAT_RGBA,
   PUD_PIXEL_FORMAT_ARGB,
} Pud_Pixel_Format;

/**
 * @typedef Pud_Player
 * Type that holds a player ID
 */
typedef enum
{
   PUD_PLAYER_RED       = 0, /**< Player 1 (or Red) */
   PUD_PLAYER_BLUE      = 1, /**< Player 2 (or Blue) */
   PUD_PLAYER_GREEN     = 2, /**< Player 3 (or Green) */
   PUD_PLAYER_VIOLET    = 3, /**< Player 4 (or Violet) */
   PUD_PLAYER_ORANGE    = 4, /**< Player 5 (or Orange) */
   PUD_PLAYER_BLACK     = 5, /**< Player 6 (or Black) */
   PUD_PLAYER_WHITE     = 6, /**< Player 7 (or White) */
   PUD_PLAYER_YELLOW    = 7, /**< Player 8 (or Yellow) */

   PUD_PLAYER_NEUTRAL   = 15 /** Neutral player */
} Pud_Player;

typedef enum
{
   PUD_UPGRADE_SWORD_1          = 0,
   PUD_UPGRADE_SWORD_2,
   PUD_UPGRADE_AXE_1,
   PUD_UPGRADE_AXE_2,
   PUD_UPGRADE_ARROW_1,
   PUD_UPGRADE_ARROW_2,
   PUD_UPGRADE_SPEAR_1,
   PUD_UPGRADE_SPEAR_2,
   PUD_UPGRADE_HUMAN_SHIELD_1,
   PUD_UPGRADE_HUMAN_SHIELD_2,
   PUD_UPGRADE_ORC_SHIELD_1,
   PUD_UPGRADE_ORC_SHIELD_2,
   PUD_UPGRADE_HUMAN_SHIP_CANNON_1,
   PUD_UPGRADE_HUMAN_SHIP_CANNON_2,
   PUD_UPGRADE_ORC_SHIP_CANNON_1,
   PUD_UPGRADE_ORC_SHIP_CANNON_2,
   PUD_UPGRADE_HUMAN_SHIP_ARMOR_1,
   PUD_UPGRADE_HUMAN_SHIP_ARMOR_2,
   PUD_UPGRADE_ORC_SHIP_ARMOR_1,
   PUD_UPGRADE_ORC_SHIP_ARMOR_2,
   PUD_UPGRADE_CATAPULT_1,
   PUD_UPGRADE_CATAPULT_2,
   PUD_UPGRADE_BALLISTA_1,
   PUD_UPGRADE_BALLISTA_2,
   PUD_UPGRADE_TRAIN_RANGERS,
   PUD_UPGRADE_LONGBOW,
   PUD_UPGRADE_RANGER_SCOUTING,
   PUD_UPGRADE_RANGER_MARKSMANSHIP,
   PUD_UPGRADE_TRAIN_BERSERKERS,
   PUD_UPGRADE_LIGHTER_AXES,
   PUD_UPGRADE_BERSERKER_SCOUTING,
   PUD_UPGRADE_BERSERKER_REGENERATION,
   PUD_UPGRADE_TRAIN_OGRE_MAGES,
   PUD_UPGRADE_TRAIN_PALADINS,
   PUD_UPGRADE_HOLY_VISION,
   PUD_UPGRADE_HEALING,
   PUD_UPGRADE_EXORCISM,
   PUD_UPGRADE_FLAME_SHIELD,
   PUD_UPGRADE_FIREBALL,
   PUD_UPGRADE_SLOW,
   PUD_UPGRADE_INVISIBILITY,
   PUD_UPGRADE_POLYMORPH,
   PUD_UPGRADE_BLIZZARD,
   PUD_UPGRADE_EYE_OF_KILROGG,
   PUD_UPGRADE_BLOODLUST,
   PUD_UPGRADE_RAISE_DEAD,
   PUD_UPGRADE_DEATH_COIL,
   PUD_UPGRADE_WHIRLWIND,
   PUD_UPGRADE_HASTE,
   PUD_UPGRADE_UNHOLY_ARMOR,
   PUD_UPGRADE_RUNES,
   PUD_UPGRADE_DEATH_AND_DECAY
} Pud_Upgrade;

typedef enum
{
   PUD_ICON_PEASANT                              = 0,
   PUD_ICON_PEON                                 = 1,
   PUD_ICON_FOOTMAN                              = 2,
   PUD_ICON_GRUNT                                = 3,
   PUD_ICON_ARCHER                               = 4,
   PUD_ICON_AXETHROWER                           = 5,
   PUD_ICON_RANGER                               = 6,
   PUD_ICON_BERSERKER                            = 7,
   PUD_ICON_KNIGHT                               = 8,
   PUD_ICON_OGRE                                 = 9,
   PUD_ICON_PALADIN                              = 10,
   PUD_ICON_OGRE_MAGE                            = 11,
   PUD_ICON_DWARVES                              = 12,
   PUD_ICON_GOBLIN_SAPPER                        = 13,
   PUD_ICON_MAGE                                 = 14,
   PUD_ICON_DEATH_KNIGHT                         = 15,
   PUD_ICON_BALLISTA                             = 16,
   PUD_ICON_CATAPULT                             = 17,
   PUD_ICON_HUMAN_TANKER                         = 18,
   PUD_ICON_ORC_TANKER                           = 19,
   PUD_ICON_HUMAN_TRANSPORT                      = 20,
   PUD_ICON_ORC_TRANSPORT                        = 21,
   PUD_ICON_ELVEN_DESTROYER                      = 22,
   PUD_ICON_TROLL_DESTROYER                      = 23,
   PUD_ICON_BATTLESHIP                           = 24,
   PUD_ICON_JUGGERNAUGHT                         = 25,
   PUD_ICON_GNOMISH_SUBMARINE                    = 26,
   PUD_ICON_GIANT_TURTLE                         = 27,
   PUD_ICON_GNOMISH_FLYING_MACHINE               = 28,
   PUD_ICON_GOBLIN_ZEPPLIN                       = 29,
   PUD_ICON_GRYPHON_RIDER                        = 30,
   PUD_ICON_DRAGON                               = 31,
   PUD_ICON_LOTHAR                               = 32,
   PUD_ICON_GUL_DAN                              = 33,
   PUD_ICON_UTHER_LIGHTBRINGER                   = 34,
   PUD_ICON_ZUL_JIN                              = 35,
   PUD_ICON_CHO_GALL                             = 36,
   PUD_ICON_DAEMON                               = 37,
   PUD_ICON_FARM                                 = 38,
   PUD_ICON_PIG_FARM                             = 39,
   PUD_ICON_TOWN_HALL                            = 40,
   PUD_ICON_GREAT_HALL                           = 41,
   PUD_ICON_HUMAN_BARRACKS                       = 42,
   PUD_ICON_ORC_BARRACKS                         = 43,
   PUD_ICON_ELVEN_LUMBER_MILL                    = 44,
   PUD_ICON_TROLL_LUMBER_MILL                    = 45,
   PUD_ICON_HUMAN_BLACKSMITH                     = 46,
   PUD_ICON_ORC_BLACKSMITH                       = 47,
   PUD_ICON_HUMAN_SHIPYARD                       = 48,
   PUD_ICON_ORC_SHIPYARD                         = 49,
   PUD_ICON_HUMAN_REFINERY                       = 50,
   PUD_ICON_ORC_REFINERY                         = 51,
   PUD_ICON_HUMAN_FOUNDRY                        = 52,
   PUD_ICON_ORC_FOUNDRY                          = 53,
   PUD_ICON_HUMAN_OIL_WELL                       = 54,
   PUD_ICON_ORC_OIL_WELL                         = 55,
   PUD_ICON_STABLES                              = 56,
   PUD_ICON_OGRE_MOUND                           = 57,
   PUD_ICON_GNOMISH_INVENTOR                     = 58,
   PUD_ICON_GOBLIN_ALCHEMIST                     = 59,
   PUD_ICON_HUMAN_SCOUT_TOWER                    = 60,
   PUD_ICON_ORC_SCOUT_TOWER                      = 61,
   PUD_ICON_CHURCH                               = 62,
   PUD_ICON_ALTAR_OF_STORMS                      = 63,
   PUD_ICON_MAGE_TOWER                           = 64,
   PUD_ICON_TEMPLE_OF_THE_DAMNED                 = 65,
   PUD_ICON_KEEP                                 = 66,
   PUD_ICON_STRONGHOLD                           = 67,
   PUD_ICON_CASTLE                               = 68,
   PUD_ICON_FORTRESS                             = 69,
   PUD_ICON_GRYPHON_AVIARY                       = 72,
   PUD_ICON_DRAGON_ROOST                         = 73,
   PUD_ICON_GOLD_MINE                            = 74,
   PUD_ICON_HUMAN_GUARD_TOWER                    = 75,
   PUD_ICON_HUMAN_CANNON_TOWER                   = 76,
   PUD_ICON_ORC_GUARD_TOWER                      = 77,
   PUD_ICON_ORC_CANNON_TOWER                     = 78,
   PUD_ICON_OIL_PATCH                            = 79,
   PUD_ICON_DARK_PORTAL                          = 80,
   PUD_ICON_CIRCLE_OF_POWER                      = 81,
   PUD_ICON_RUNESTONE                            = 82,
   PUD_ICON_CANCEL                               = 91,
   PUD_ICON_HUMAN_WALL                           = 92,
   PUD_ICON_ORC_WALL                             = 93,
   PUD_ICON_EYE_OF_KILROGG                       = 111,
   PUD_ICON_SKELETON                             = 114,
   PUD_ICON_CRITTER                              = 115,
   PUD_ICON_CRITTER_SHEEP                        = PUD_ICON_CRITTER,
   PUD_ICON_CRITTER_SEAL                         = PUD_ICON_CRITTER,
   PUD_ICON_CRITTER_PIG                          = PUD_ICON_CRITTER,
   PUD_ICON_CRITTER_RED_PIG                      = PUD_ICON_CRITTER,
   PUD_ICON_KARGATH_BLADEFIST                    = 186,
   PUD_ICON_ALLERIA                              = 187,
   PUD_ICON_DANATH                               = 188,
   PUD_ICON_TERON_GOREFIEND                      = 189,
   PUD_ICON_GROM_HELLSCREAM                      = 190,
   PUD_ICON_KURDRAN_AND_SKY_REE                  = 191,
   PUD_ICON_DEATHWING                            = 192,
   PUD_ICON_KHADGAR                              = 193,
   PUD_ICON_DENTARG                              = 194,
   PUD_ICON_TURALYON                             = 195,
   PUD_ICON_ATTACK_PEON                          = PUD_ICON_PEON,
   PUD_ICON_ATTACK_PEASANT                       = PUD_ICON_PEASANT,

   PUD_ICON_SWORD_1                              = 117,
   PUD_ICON_SWORD_2                              = 118,
   PUD_ICON_AXE_1                                = 120,
   PUD_ICON_AXE_2                                = 121,
   PUD_ICON_ARROW_1                              = 125,
   PUD_ICON_ARROW_2                              = 126,
   PUD_ICON_SPEAR_1                              = 128,
   PUD_ICON_SPEAR_2                              = 129,
   PUD_ICON_HUMAN_SHIELD_1                       = 165,
   PUD_ICON_HUMAN_SHIELD_2                       = 166,
   PUD_ICON_ORC_SHIELD_1                         = 168,
   PUD_ICON_ORC_SHIELD_2                         = 169,
   PUD_ICON_HUMAN_SHIP_CANNON_1                  = 145,
   PUD_ICON_HUMAN_SHIP_CANNON_2                  = 146,
   PUD_ICON_ORC_SHIP_CANNON_1                    = 148,
   PUD_ICON_ORC_SHIP_CANNON_2                    = 149,
   PUD_ICON_HUMAN_SHIP_ARMOR_1                   = 154,
   PUD_ICON_HUMAN_SHIP_ARMOR_2                   = 155,
   PUD_ICON_ORC_SHIP_ARMOR_1                     = 151,
   PUD_ICON_ORC_SHIP_ARMOR_2                     = 152,
   PUD_ICON_CATAPULT_1                           = 140,
   PUD_ICON_CATAPULT_2                           = 141,
   PUD_ICON_BALLISTA_1                           = 138,
   PUD_ICON_BALLISTA_2                           = 139,
   PUD_ICON_TRAIN_RANGERS                        = 6,
   PUD_ICON_LONGBOW                              = 132,
   PUD_ICON_RANGER_SCOUTING                      = 133,
   PUD_ICON_RANGER_MARKSMANSHIP                  = 134,
   PUD_ICON_TRAIN_BERSERKERS                     = 7,
   PUD_ICON_LIGHTER_AXES                         = 135,
   PUD_ICON_BERSERKER_SCOUTING                   = 136,
   PUD_ICON_BERSERKER_REGENERATION               = 137,
   PUD_ICON_TRAIN_OGRE_MAGES                     = 11,
   PUD_ICON_TRAIN_PALADINS                       = 10,
   PUD_ICON_HOLY_VISION                          = 106,
   PUD_ICON_HEALING                              = 107,
   PUD_ICON_EXORCISM                             = 110,
   PUD_ICON_FLAME_SHIELD                         = 100,
   PUD_ICON_FIREBALL                             = 101,
   PUD_ICON_SLOW                                 = 94,
   PUD_ICON_INVISIBILITY                         = 95,
   PUD_ICON_POLYMORPH                            = 115,
   PUD_ICON_BLIZZARD                             = 105,
   PUD_ICON_BLOODLUST                            = 112,
   PUD_ICON_RAISE_DEAD                           = 114,
   PUD_ICON_DEATH_COIL                           = 103,
   PUD_ICON_WHIRLWIND                            = 104,
   PUD_ICON_HASTE                                = 96,
   PUD_ICON_UNHOLY_ARMOR                         = 98,
   PUD_ICON_RUNES                                = 97,
   PUD_ICON_DEATH_AND_DECAY                      = 108,
} Pud_Icon;

typedef enum
{
   PUD_UNIT_FOOTMAN                     = 0x00,
   PUD_UNIT_GRUNT                       = 0x01,
   PUD_UNIT_PEASANT                     = 0x02,
   PUD_UNIT_PEON                        = 0x03,
   PUD_UNIT_BALLISTA                    = 0x04,
   PUD_UNIT_CATAPULT                    = 0x05,
   PUD_UNIT_KNIGHT                      = 0x06,
   PUD_UNIT_OGRE                        = 0x07,
   PUD_UNIT_ARCHER                      = 0x08,
   PUD_UNIT_AXETHROWER                  = 0x09,
   PUD_UNIT_MAGE                        = 0x0a,
   PUD_UNIT_DEATH_KNIGHT                = 0x0b,
   PUD_UNIT_PALADIN                     = 0x0c,
   PUD_UNIT_OGRE_MAGE                   = 0x0d,
   PUD_UNIT_DWARVES                     = 0x0e,
   PUD_UNIT_GOBLIN_SAPPER               = 0x0f,
   PUD_UNIT_ATTACK_PEASANT              = 0x10,
   PUD_UNIT_ATTACK_PEON                 = 0x11,
   PUD_UNIT_RANGER                      = 0x12,
   PUD_UNIT_BERSERKER                   = 0x13,
   PUD_UNIT_ALLERIA                     = 0x14,
   PUD_UNIT_TERON_GOREFIEND             = 0x15,
   PUD_UNIT_KURDRAN_AND_SKY_REE         = 0x16,
   PUD_UNIT_DENTARG                     = 0x17,
   PUD_UNIT_KHADGAR                     = 0x18,
   PUD_UNIT_GROM_HELLSCREAM             = 0x19,
   PUD_UNIT_HUMAN_TANKER                = 0x1a,
   PUD_UNIT_ORC_TANKER                  = 0x1b,
   PUD_UNIT_HUMAN_TRANSPORT             = 0x1c,
   PUD_UNIT_ORC_TRANSPORT               = 0x1d,
   PUD_UNIT_ELVEN_DESTROYER             = 0x1e,
   PUD_UNIT_TROLL_DESTROYER             = 0x1f,
   PUD_UNIT_BATTLESHIP                  = 0x20,
   PUD_UNIT_JUGGERNAUGHT                = 0x21,
   PUD_UNIT_DEATHWING                   = 0x23,
   PUD_UNIT_GNOMISH_SUBMARINE           = 0x26,
   PUD_UNIT_GIANT_TURTLE                = 0x27,
   PUD_UNIT_GNOMISH_FLYING_MACHINE      = 0x28,
   PUD_UNIT_GOBLIN_ZEPPLIN              = 0x29,
   PUD_UNIT_GRYPHON_RIDER               = 0x2a,
   PUD_UNIT_DRAGON                      = 0x2b,
   PUD_UNIT_TURALYON                    = 0x2c,
   PUD_UNIT_EYE_OF_KILROGG              = 0x2d,
   PUD_UNIT_DANATH                      = 0x2e,
   PUD_UNIT_KARGATH_BLADEFIST           = 0x2f,
   PUD_UNIT_CHO_GALL                    = 0x31,
   PUD_UNIT_LOTHAR                      = 0x32,
   PUD_UNIT_GUL_DAN                     = 0x33,
   PUD_UNIT_UTHER_LIGHTBRINGER          = 0x34,
   PUD_UNIT_ZUL_JIN                     = 0x35,
   PUD_UNIT_SKELETON                    = 0x37,
   PUD_UNIT_DAEMON                      = 0x38,
   PUD_UNIT_CRITTER                     = 0x39,
   PUD_UNIT_FARM                        = 0x3a,
   PUD_UNIT_PIG_FARM                    = 0x3b,
   PUD_UNIT_HUMAN_BARRACKS              = 0x3c,
   PUD_UNIT_ORC_BARRACKS                = 0x3d,
   PUD_UNIT_CHURCH                      = 0x3e,
   PUD_UNIT_ALTAR_OF_STORMS             = 0x3f,
   PUD_UNIT_HUMAN_SCOUT_TOWER           = 0x40,
   PUD_UNIT_ORC_SCOUT_TOWER             = 0x41,
   PUD_UNIT_STABLES                     = 0x42,
   PUD_UNIT_OGRE_MOUND                  = 0x43,
   PUD_UNIT_GNOMISH_INVENTOR            = 0x44,
   PUD_UNIT_GOBLIN_ALCHEMIST            = 0x45,
   PUD_UNIT_GRYPHON_AVIARY              = 0x46,
   PUD_UNIT_DRAGON_ROOST                = 0x47,
   PUD_UNIT_HUMAN_SHIPYARD              = 0x48,
   PUD_UNIT_ORC_SHIPYARD                = 0x49,
   PUD_UNIT_TOWN_HALL                   = 0x4a,
   PUD_UNIT_GREAT_HALL                  = 0x4b,
   PUD_UNIT_ELVEN_LUMBER_MILL           = 0x4c,
   PUD_UNIT_TROLL_LUMBER_MILL           = 0x4d,
   PUD_UNIT_HUMAN_FOUNDRY               = 0x4e,
   PUD_UNIT_ORC_FOUNDRY                 = 0x4f,
   PUD_UNIT_MAGE_TOWER                  = 0x50,
   PUD_UNIT_TEMPLE_OF_THE_DAMNED        = 0x51,
   PUD_UNIT_HUMAN_BLACKSMITH            = 0x52,
   PUD_UNIT_ORC_BLACKSMITH              = 0x53,
   PUD_UNIT_HUMAN_REFINERY              = 0x54,
   PUD_UNIT_ORC_REFINERY                = 0x55,
   PUD_UNIT_HUMAN_OIL_WELL              = 0x56,
   PUD_UNIT_ORC_OIL_WELL                = 0x57,
   PUD_UNIT_KEEP                        = 0x58,
   PUD_UNIT_STRONGHOLD                  = 0x59,
   PUD_UNIT_CASTLE                      = 0x5a,
   PUD_UNIT_FORTRESS                    = 0x5b,
   PUD_UNIT_GOLD_MINE                   = 0x5c,
   PUD_UNIT_OIL_PATCH                   = 0x5d,
   PUD_UNIT_HUMAN_START                 = 0x5e,
   PUD_UNIT_ORC_START                   = 0x5f,
   PUD_UNIT_HUMAN_GUARD_TOWER           = 0x60,
   PUD_UNIT_ORC_GUARD_TOWER             = 0x61,
   PUD_UNIT_HUMAN_CANNON_TOWER          = 0x62,
   PUD_UNIT_ORC_CANNON_TOWER            = 0x63,
   PUD_UNIT_CIRCLE_OF_POWER             = 0x64,
   PUD_UNIT_DARK_PORTAL                 = 0x65,
   PUD_UNIT_RUNESTONE                   = 0x66,
   PUD_UNIT_HUMAN_WALL                  = 0x67,
   PUD_UNIT_ORC_WALL                    = 0x68,

   /* Custom types */
   PUD_UNIT_CRITTER_SHEEP               = 0x69,
   PUD_UNIT_CRITTER_PIG                 = 0x6a,
   PUD_UNIT_CRITTER_SEAL                = 0x6b,
   PUD_UNIT_CRITTER_RED_PIG             = 0x6c,

   /* Sentinel. Do not touch 0x7f. war2edit heavily relies on this value */
   PUD_UNIT_NONE                        = 0x7f
} Pud_Unit;

typedef enum
{
   PUD_OWNER_NOBODY                     = 0x03,
   PUD_OWNER_PASSIVE_COMPUTER           = 0x02,
   PUD_OWNER_COMPUTER                   = 0x04,
   PUD_OWNER_HUMAN                      = 0x05,
   PUD_OWNER_RESCUE_PASSIVE             = 0x06,
   PUD_OWNER_RESCUE_ACTIVE              = 0x07
} Pud_Owner;

typedef enum
{
   PUD_PROJECTILE_LIGHTNING             = 0x00,
   PUD_PROJECTILE_GRIFFON_HAMMER        = 0x01,
   PUD_PROJECTILE_DRAGON_BREATH         = 0x02,
   PUD_PROJECTILE_FLAME_SHIELD          = 0x03,
   PUD_PROJECTILE_FLAME_SHIELD_SELF     = 0x04,
   PUD_PROJECTILE_BLIZZARD              = 0x05,
   PUD_PROJECTILE_DEATH_AND_DECAY       = 0x06,
   PUD_PROJECTILE_BIG_CANNON            = 0x07,
   PUD_PROJECTILE_BLACK_POWDER          = 0x08,
   PUD_PROJECTILE_HEAL_EFFECT           = 0x09,
   PUD_PROJECTILE_TOUCH_OF_DEATH        = 0x0a,
   PUD_PROJECTILE_RUNE                  = 0x0b,
   PUD_PROJECTILE_TORNADO               = 0x0c,
   PUD_PROJECTILE_CATAPULT_ROCK         = 0x0d,
   PUD_PROJECTILE_BALLISTA_BOLT         = 0x0e,
   PUD_PROJECTILE_ARROW                 = 0x0f,
   PUD_PROJECTILE_AXE                   = 0x10,
   PUD_PROJECTILE_SUBMARINE_MISSILE     = 0x11,
   PUD_PROJECTILE_TURTLE_MISSILE        = 0x12,
   PUD_PROJECTILE_DARK_FLAME            = 0x13,
   PUD_PROJECTILE_BRIGHT_FLAME          = 0x14,
   PUD_PROJECTILE_BLOOD                 = 0x15,
   PUD_PROJECTILE_BLACK_POWDER_MORE     = 0x16,
   PUD_PROJECTILE_EXPLOSION             = 0x17,
   PUD_PROJECTILE_SMALL_CANNON          = 0x18,
   PUD_PROJECTILE_METAL_SPARK           = 0x19,
   PUD_PROJECTILE_MINI_EXPLOSION        = 0x1a,
   PUD_PROJECTILE_DAEMON_FIRE           = 0x1b,
   PUD_PROJECTILE_GREEN_CROSS           = 0x1c,
   PUD_PROJECTILE_NONE                  = 0x1d,
} Pud_Projectile;

typedef uint32_t Pud_Allow;

typedef enum
{
   PUD_SIDE_HUMAN                       = 0x00,
   PUD_SIDE_ORC                         = 0x01,
   PUD_SIDE_NEUTRAL                     = 0x02
} Pud_Side;

typedef enum
{
   PUD_OPEN_MODE_R = (1 << 0),
   PUD_OPEN_MODE_W = (1 << 1),
   PUD_OPEN_MODE_RW = (PUD_OPEN_MODE_R | PUD_OPEN_MODE_W)
} Pud_Open_Mode;

typedef enum
{
   PUD_ERA_FOREST       = 0,
   PUD_ERA_WINTER       = 1,
   PUD_ERA_WASTELAND    = 2,
   PUD_ERA_SWAMP        = 3
} Pud_Era;

typedef enum
{
   PUD_AI_LAND_ATTACK    = 0x00,
   PUD_AI_PASSIVE        = 0x01,
   PUD_AI_SEA_ATTACK     = 0x19,
   PUD_AI_AIR_ATTACK     = 0x1a,

   PUD_AI_ORC_3          = 0x02,
   PUD_AI_ORC_4          = 0x04,
   PUD_AI_ORC_5          = 0x06,
   PUD_AI_ORC_6          = 0x08,
   PUD_AI_ORC_7          = 0x0a,
   PUD_AI_ORC_8          = 0x0c,
   PUD_AI_ORC_9          = 0x0e,
   PUD_AI_ORC_10         = 0x10,
   PUD_AI_ORC_11         = 0x12,
   PUD_AI_ORC_12         = 0x14,
   PUD_AI_ORC_13         = 0x16,
   PUD_AI_HUMAN_4        = 0x03,
   PUD_AI_HUMAN_5        = 0x05,
   PUD_AI_HUMAN_6        = 0x07,
   PUD_AI_HUMAN_7        = 0x09,
   PUD_AI_HUMAN_8        = 0x0b,
   PUD_AI_HUMAN_9        = 0x0d,
   PUD_AI_HUMAN_10       = 0x0f,
   PUD_AI_HUMAN_11       = 0x11,
   PUD_AI_HUMAN_12       = 0x13,
   PUD_AI_HUMAN_13       = 0x15,

   PUD_AI_HUMAN_14_RED   = 0x1b,
   PUD_AI_HUMAN_14_WHITE = 0x1c,
   PUD_AI_HUMAN_14_BLACK = 0x1d,
   PUD_AI_ORC_14_GREEN   = 0x1e,
   PUD_AI_ORC_14_WHITE   = 0x1f,

   PUD_AI_EXPANSION_1    = 0x20,
   PUD_AI_EXPANSION_51   = 0x52
} Pud_AI;


/*
 * Allow unit bitfields
 */
#define PUD_ALLOW_UNIT_FOOTMAN_GRUNT                    ((uint_fast32_t)1 << (uint_fast32_t)0)
#define PUD_ALLOW_UNIT_PEASANT_PEON                     ((uint_fast32_t)1 << (uint_fast32_t)1)
#define PUD_ALLOW_UNIT_BALLISTA_CATAPULT                ((uint_fast32_t)1 << (uint_fast32_t)2)
#define PUD_ALLOW_UNIT_KNIGHT_OGRE                      ((uint_fast32_t)1 << (uint_fast32_t)3)
#define PUD_ALLOW_UNIT_ARCHER_AXETHROWER                ((uint_fast32_t)1 << (uint_fast32_t)4)
#define PUD_ALLOW_UNIT_MAGE_DEATH_KNIGHT                ((uint_fast32_t)1 << (uint_fast32_t)5)
#define PUD_ALLOW_UNIT_TANKER                           ((uint_fast32_t)1 << (uint_fast32_t)6)
#define PUD_ALLOW_UNIT_DESTROYER                        ((uint_fast32_t)1 << (uint_fast32_t)7)
#define PUD_ALLOW_UNIT_TRANSPORT                        ((uint_fast32_t)1 << (uint_fast32_t)8)
#define PUD_ALLOW_UNIT_BATTLESHIP_JUGGERNAUGHT          ((uint_fast32_t)1 << (uint_fast32_t)9)
#define PUD_ALLOW_UNIT_SUBMARINE_TURTLE                 ((uint_fast32_t)1 << (uint_fast32_t)10)
#define PUD_ALLOW_UNIT_FLYING_MACHINE_ZEPLIN            ((uint_fast32_t)1 << (uint_fast32_t)11)
#define PUD_ALLOW_UNIT_GRYPHON_DRAGON                   ((uint_fast32_t)1 << (uint_fast32_t)12)
/* Bit 13 is unused */
#define PUD_ALLOW_UNIT_DEMOLITION_SQUAD_SAPPERS         ((uint_fast32_t)1 << (uint_fast32_t)14)
#define PUD_ALLOW_UNIT_AVIARY_ROOST                     ((uint_fast32_t)1 << (uint_fast32_t)15)
#define PUD_ALLOW_UNIT_FARM                             ((uint_fast32_t)1 << (uint_fast32_t)16)
#define PUD_ALLOW_UNIT_BARRACKS                         ((uint_fast32_t)1 << (uint_fast32_t)17)
#define PUD_ALLOW_UNIT_LUMBER_MILL                      ((uint_fast32_t)1 << (uint_fast32_t)18)
#define PUD_ALLOW_UNIT_STABLES_MOUND                    ((uint_fast32_t)1 << (uint_fast32_t)19)
#define PUD_ALLOW_UNIT_MAGE_TOWER_TEMPLE                ((uint_fast32_t)1 << (uint_fast32_t)20)
#define PUD_ALLOW_UNIT_FOUNDRY                          ((uint_fast32_t)1 << (uint_fast32_t)21)
#define PUD_ALLOW_UNIT_REFINERY                         ((uint_fast32_t)1 << (uint_fast32_t)22)
#define PUD_ALLOW_UNIT_INVENTOR_ALCHEMIST               ((uint_fast32_t)1 << (uint_fast32_t)23)
#define PUD_ALLOW_UNIT_CHURCH_ALTAR_STORMS              ((uint_fast32_t)1 << (uint_fast32_t)24)
#define PUD_ALLOW_UNIT_TOWERS                           ((uint_fast32_t)1 << (uint_fast32_t)25)
#define PUD_ALLOW_UNIT_TOWN_HALL_GREAT_HALL             ((uint_fast32_t)1 << (uint_fast32_t)26)
#define PUD_ALLOW_UNIT_KEEP_STRONGHOLD                  ((uint_fast32_t)1 << (uint_fast32_t)27)
#define PUD_ALLOW_UNIT_CASTLE_FORTRESS                  ((uint_fast32_t)1 << (uint_fast32_t)28)
#define PUD_ALLOW_UNIT_BLACKSMITH                       ((uint_fast32_t)1 << (uint_fast32_t)29)
#define PUD_ALLOW_UNIT_SHIPYARD                         ((uint_fast32_t)1 << (uint_fast32_t)30)
/* Bit 31 is unused */
#define PUD_ALLOW_UNIT_MARK                             ((uint_fast32_t)1 << (uint_fast32_t)31)


/*
 * Allow unit spells
 */
#define PUD_ALLOW_SPELL_HOLY_VISION                     ((uint_fast32_t)1 << (uint_fast32_t)0)
#define PUD_ALLOW_SPELL_HEALING                         ((uint_fast32_t)1 << (uint_fast32_t)1)
/* Bit 2 is unused */
#define PUD_ALLOW_SPELL_EXORCISM                        ((uint_fast32_t)1 << (uint_fast32_t)3)
#define PUD_ALLOW_SPELL_FLAME_SHIELD                    ((uint_fast32_t)1 << (uint_fast32_t)4)
#define PUD_ALLOW_SPELL_FIREBALL                        ((uint_fast32_t)1 << (uint_fast32_t)5)
#define PUD_ALLOW_SPELL_SLOW                            ((uint_fast32_t)1 << (uint_fast32_t)6)
#define PUD_ALLOW_SPELL_INVISIBILITY                    ((uint_fast32_t)1 << (uint_fast32_t)7)
#define PUD_ALLOW_SPELL_POLYMORPH                       ((uint_fast32_t)1 << (uint_fast32_t)8)
#define PUD_ALLOW_SPELL_BLIZZARD                        ((uint_fast32_t)1 << (uint_fast32_t)9)
#define PUD_ALLOW_SPELL_EYE_OF_KILROGG                  ((uint_fast32_t)1 << (uint_fast32_t)10)
#define PUD_ALLOW_SPELL_BLOODLUST                       ((uint_fast32_t)1 << (uint_fast32_t)11)
/* Bit 12 is unused */
#define PUD_ALLOW_SPELL_RAISE_DEAD                      ((uint_fast32_t)1 << (uint_fast32_t)13)
#define PUD_ALLOW_SPELL_DEATH_COIL                      ((uint_fast32_t)1 << (uint_fast32_t)14)
#define PUD_ALLOW_SPELL_WHIRLWIND                       ((uint_fast32_t)1 << (uint_fast32_t)15)
#define PUD_ALLOW_SPELL_HASTE                           ((uint_fast32_t)1 << (uint_fast32_t)16)
#define PUD_ALLOW_SPELL_UNHOLY_ARMOR                    ((uint_fast32_t)1 << (uint_fast32_t)17)
#define PUD_ALLOW_SPELL_RUNES                           ((uint_fast32_t)1 << (uint_fast32_t)18)
#define PUD_ALLOW_SPELL_DEATH_AND_DECAY                 ((uint_fast32_t)1 << (uint_fast32_t)19)
/* Bits 20 to 31 are unused */
#define PUD_ALLOW_SPELL_MARK                            ((uint_fast32_t)1 << (uint_fast32_t)30)

typedef struct _Pud Pud;
typedef struct _Pud_Unit_Data Pud_Unit_Data;
typedef struct _Pud_Unit_Characteristics Pud_Unit_Characteristics;
typedef struct _Pud_Upgrade_Characteristics Pud_Upgrade_Characteristics;

struct _Pud_Upgrade_Characteristics
{
   uint8_t           time;
   uint16_t          gold;
   uint16_t          lumber;
   uint16_t          oil;
   uint16_t          icon;
   uint16_t          group;
   uint32_t          flags;
};

struct _Pud_Unit_Characteristics
{
   uint32_t     sight;
   uint16_t     hp;
   uint8_t      build_time;
   uint8_t      gold_cost;
   uint8_t      lumber_cost;
   uint8_t      oil_cost;
   uint8_t      range;
   uint8_t      computer_react_range;
   uint8_t      human_react_range;
   uint8_t      armor;
   uint8_t      basic_damage;
   uint8_t      piercing_damage;
   uint8_t      decay_rate;
   uint8_t      annoy;
   uint8_t      has_magic;
   uint8_t      weapons_upgradable;
   uint8_t      armor_upgradable;

   uint8_t      missile_weapon;
   uint8_t      type;
   uint8_t      mouse_right_btn;
   uint8_t      rect_sel;
   uint8_t      priority;
   uint16_t     point_value;
   uint8_t      can_target;
   uint32_t     flags;
   uint16_t     overlap_frames;
   uint16_t     size_w;
   uint16_t     size_h;
   uint16_t     box_w;
   uint16_t     box_h;
};

struct _Pud_Unit_Data
{
   uint16_t x;
   uint16_t y;
   uint8_t  type;
   uint8_t  owner;
   uint16_t alter;
};

struct _Pud
{
   Pud_Open_Mode  open_mode;
   unsigned char *mem_map;
   unsigned char *ptr;
   size_t         mem_map_size;
   char          *filename;

   uint32_t       tag;
   uint16_t       version;
   char           description[32];
   uint8_t        era;
   Pud_Dimensions dims;


   /**
    * @struct
    * Contains the the owners of each players
    * All values are of type #Pud_Owner
    */
   struct {
      uint8_t players[8];
      uint8_t unusable[7];
      uint8_t neutral;
   } owner; /* [defaults] */

   struct {
      uint8_t players[8];
      uint8_t unusable[7];
      uint8_t neutral;
   } side; /* [defaults] */

   struct {
      uint16_t players[8];
      uint16_t unusable[7];
      uint16_t neutral;
   } sgld, slbr, soil; /* [defaults] */

   struct {
      uint8_t players[8];
      uint8_t unusable[7];
      uint8_t neutral;
   } ai; /* [defaults] */

   struct _alow {
      Pud_Allow players[8];
      Pud_Allow unusable[7];
      Pud_Allow neutral;
   } unit_alow, spell_start, spell_alow, spell_acq, up_alow, up_acq; /* [defaults] */

   Pud_Upgrade_Characteristics upgrade[52]; /* [defaults] */

   /* Cache values */
   unsigned int map_w;
   unsigned int map_h;
   unsigned int tiles; /* pud->map_w * pud->map_h */

   /* Number of elements is in 'tiles' */
   uint16_t *tiles_map;
   uint16_t *action_map;
   uint16_t *movement_map;
   uint8_t *oil_map;

   Pud_Unit_Data *units;

   unsigned int units_count;

   Pud_Unit_Characteristics unit_data[110]; /* [defaults] */

   uint16_t unkwown[508];

   /* Bitfield: is section X present? */
   uint32_t     sections;

   /* Cache */
   uint8_t       current_section;

   Pud_Bool has_erax;

   unsigned int  verbose        : 3;
   unsigned int  init           : 1; /* set by defaults */
   unsigned int  default_allow  : 1; /* [defaults] */
   unsigned int  default_udta   : 1; /* [defaults] */
   unsigned int  default_ugrd   : 1; /* [defaults] */
   unsigned int  extension_pack : 1;
};

typedef struct _Pud_Color Pud_Color;

struct _Pud_Color
{
   unsigned char r;
   unsigned char g;
   unsigned char b;
   unsigned char a;
};

typedef enum
{
   PUD_ERROR_NONE = 0,
   PUD_ERROR_UNDEFINED,
   PUD_ERROR_NOT_ENOUGH_START_LOCATIONS,
   PUD_ERROR_TOO_MUCH_START_LOCATIONS,
   PUD_ERROR_NOT_INITIALIZED,
   PUD_ERROR_EMPTY_PLAYER,
   PUD_ERROR_INVALID_PLAYER,
   PUD_ERROR_NO_START_LOCATION,
} Pud_Error;

typedef struct
{
   Pud_Error type;
   union {

      Pud_Player player;
      unsigned int count;
      const Pud_Unit_Data *unit;

   } data;
} Pud_Error_Description;

Pud_Bool pud_init(void);
void pud_shutdown(void);

Pud *pud_open_new(const char *file, Pud_Open_Mode mode);
Pud *pud_open(const char *file, Pud_Open_Mode mode);
void pud_close(Pud *pud);
Pud_Bool pud_reopen(Pud *pud, const char *file, Pud_Open_Mode mode);
Pud_Bool pud_parse(Pud *pud);
void pud_verbose_set(Pud *pud, int lvl);
Pud_Bool pud_section_is_optional(Pud_Section sec);
uint32_t pud_go_to_section(Pud *pud, Pud_Section sec);
void pud_print(Pud *pud, FILE *stream);
void pud_dimensions_to_size(Pud_Dimensions dim, unsigned int *x_ret, unsigned int *y_ret);
Pud_Owner pud_owner_convert(uint8_t code);
Pud_Side pud_side_convert(uint8_t code);
void pud_version_set(Pud *pud, uint16_t version);
void pud_description_set(Pud *pud, char descr[32]);
void pud_tag_set(Pud *pud, uint32_t tag);
Pud_Error pud_check(Pud *pud, Pud_Error_Description *err);
Pud_Bool pud_defaults_set(Pud *pud);
Pud_Bool pud_write(const Pud *pud, const char *file);
int pud_unit_add(Pud *pud, uint16_t x, uint16_t y, Pud_Player owner, Pud_Unit type, uint16_t alter);
void pud_era_set(Pud *pud, Pud_Era era);
void pud_dimensions_set(Pud *pud, Pud_Dimensions dims);
void pud_tag_generate(Pud *pud);

void *pud_mmap(const char *file, size_t *size_ret);
void pud_munmap(void *map, size_t size);

unsigned char *pud_minimap_bitmap_generate(Pud *pud, unsigned int *size_ret, Pud_Pixel_Format pfmt);

Pud_Bool pud_minimap_to_ppm(Pud *pud, const char *file);

const char *pud_section_at_index(int idx);

const char *pud_unit2str(Pud_Unit unit, Pud_Bool pretty);
const char *pud_era2str(Pud_Era era);
const char *pud_color2str(Pud_Player color);
const char *pud_projectile2str(Pud_Projectile proj);
const char *pud_upgrade2str(Pud_Upgrade upgrade);

Pud_Icon pud_upgrade_icon_get(Pud_Upgrade upgrade);

uint16_t pud_tile_at(const Pud *pud, int x, int y);

Pud_Bool pud_tile_set(Pud *pud, uint16_t x, uint16_t y, uint16_t tile);
uint16_t pud_tile_get(const Pud *pud, unsigned int x, unsigned int y);
Pud_Bool pud_unit_building_is(Pud_Unit unit);

Pud_Bool pud_unit_start_location_is(Pud_Unit unit);
Pud_Bool pud_unit_flying_is(Pud_Unit unit);
Pud_Bool pud_unit_underwater_is(Pud_Unit unit);
Pud_Bool pud_unit_land_is(Pud_Unit unit);
Pud_Bool pud_unit_marine_is(Pud_Unit unit);
Pud_Bool pud_unit_always_passive_is(Pud_Unit unit);
Pud_Bool pud_unit_oil_well_is(Pud_Unit unit);
Pud_Bool pud_unit_coast_building_is(Pud_Unit unit);
Pud_Bool pud_unit_boat_is(Pud_Unit unit);

Pud_Bool pud_unit_hero_is(Pud_Unit unit);

Pud_Color pud_tile_to_color(Pud_Era era, uint16_t tile);
Pud_Color pud_color_for_player(Pud_Player player);
Pud_Color pud_gold_mine_color_get(void);
Pud_Color pud_oil_patch_color_get(void);
Pud_Color pud_color_for_unit(Pud_Unit unit, Pud_Player player);

Pud_Side pud_unit_side_get(Pud_Unit unit);

uint8_t pud_random_get(uint16_t tile);
Pud_Icon pud_unit_icon_get(Pud_Unit unit);
Pud_Bool pud_unit_valid_is(Pud_Unit unit);
Pud_Unit pud_unit_switch_side(Pud_Unit unit);

Pud_Side pud_side_for_player(const Pud *pud, Pud_Player player);

Pud_Bool pud_allow_unit_valid_is(Pud_Allow flag);
Pud_Bool pud_allow_spell_valid_is(Pud_Allow flag);
const char *pud_allow_unit2str(Pud_Allow flag);
const char *pud_allow_spell2str(Pud_Allow flag);
const Pud_Icon *pud_allow_unit_icons_get(Pud_Allow flag);
Pud_Icon pud_allow_spell_icon_get(Pud_Allow flag);

void pud_alow_defaults_set(Pud *pud);

#ifdef __cplusplus
}
#endif
#endif /* ! _PUD_H_ */
