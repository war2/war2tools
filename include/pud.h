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


/**
 * @file pud.h
 * @brief Warcraft II PUDs manipulation library
 *
 * Pud provides ways to manipulate PUD files.
 * PUD files are Warcraft II custom map files, that
 * can be loaded into Warcraft II and distributed among
 * players.
 *
 * Copyright (c) 2014 - 2016 Jean Guyomarc'h
 */

#ifndef __LIBPUD_PUD_H__
#define __LIBPUD_PUD_H__
#ifdef __cplusplus
extern "C" { /* } For dump editors */
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>


#ifdef _WIN32
# ifdef DLL_EXPORT
#  define PUDAPI __declspec(dllexport)
# endif /* ! DLL_EXPORT */
#else /* ifdef _WIN32 */
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define PUDAPI __attribute__ ((visibility("default")))
#  endif /* if __GNUC__ >= 4 */
# endif /* ifdef __GNUC__ */
#endif /* ! _WIN32 */

#ifndef PUDAPI
/**
 * @def PUDAPI
 * Marker that a symbol is public and exported. Its value is compiler dependant.
 * @since 1.0.0
 */
# define PUDAPI
#endif


/**
 * @typedef Pud_Bool
 * Boolean type that can take two values among
 * #PUD_TRUE and #PUD_FALSE
 * @since 1.0.0
 */
typedef unsigned char Pud_Bool;

/**
 * @def PUD_TRUE
 * Boolean value for TRUE (1)
 * @since 1.0.0
 */
#define PUD_TRUE  ((Pud_Bool)(1))

/**
 * @def PUD_FALSE
 * Boolean value for FALSE (0)
 * @since 1.0.0
 */
#define PUD_FALSE ((Pud_Bool)(0))

/**
 * @def PUD_VERSION_WAR2
 * Minimum version of Warcraft 2. This is likely the value
 * you are searching for, as it will be compatible with most of
 * Warcraft 2 versions.
 * @since 1.0.0
 * @see pud_version_set()
 */
#define PUD_VERSION_WAR2                0x11

/**
 * @typedef Pud_Section
 * Type that represents a PUD section
 * @since 1.0.0
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
 * @since 1.0.0
 */
typedef enum
{
   PUD_DIMENSIONS_UNDEFINED = 0, /**< Undefined, invalid dimensions */
   PUD_DIMENSIONS_32_32, /**< 32x32 map */
   PUD_DIMENSIONS_64_64, /**< 64x64 map */
   PUD_DIMENSIONS_96_96, /**< 96x96 map */
   PUD_DIMENSIONS_128_128 /**< 128x128 map */
} Pud_Dimensions;

/**
 * @typedef Pud_Pixel_Format
 * Type that holds different pixel formats
 * @since 1.0.0
 */
typedef enum
{
   PUD_PIXEL_FORMAT_RGBA, /**< Pixels are 8 bits each, RGBA ordered */
   PUD_PIXEL_FORMAT_ARGB, /**< Pixels are 8 bits each, ARGB ordered */
} Pud_Pixel_Format;

/**
 * @typedef Pud_Player
 * Type that holds a player ID
 * @since 1.0.0
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

/**
 * @typedef Pud_Upgrade
 * Type that describes an upgrade
 * @since 1.0.0
 */
typedef enum
{
   PUD_UPGRADE_SWORD_1 = 0, /**< Human melee level 1 */
   PUD_UPGRADE_SWORD_2, /**< Human melee level 2 */
   PUD_UPGRADE_AXE_1, /**< Orc melee level 1 */
   PUD_UPGRADE_AXE_2, /**< Orc melee level 2 */
   PUD_UPGRADE_ARROW_1, /**< Elven arrows level 1 */
   PUD_UPGRADE_ARROW_2, /**< Elven arrows level 2 */
   PUD_UPGRADE_SPEAR_1, /**< Troll axes level 1 */
   PUD_UPGRADE_SPEAR_2, /**< Troll axes level 2 */
   PUD_UPGRADE_HUMAN_SHIELD_1, /**< Human shield level 1 */
   PUD_UPGRADE_HUMAN_SHIELD_2, /**< Human shield level 2 */
   PUD_UPGRADE_ORC_SHIELD_1, /**< Orc shield level 1 */
   PUD_UPGRADE_ORC_SHIELD_2, /**< Orc shield level 2 */
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

/**
 * @typedef Pud_Icon
 * Type that describes an icon
 * @since 1.0.0
 */
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
   PUD_ICON_CATAPULT_1                           = 138,
   PUD_ICON_CATAPULT_2                           = 139,
   PUD_ICON_BALLISTA_1                           = 140,
   PUD_ICON_BALLISTA_2                           = 141,
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

/**
 * @typedef Pud_Unit
 * Type that describes a unit
 * @since 1.0.0
 */
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

   /*
    * Hey, why not zero?? Because zero is taken by the
    * damn footman!
    */
   PUD_UNIT_NONE                        = 0x7f
} Pud_Unit;


/**
 * @typedef Pud_Owner
 * Owner of a Pud_Player.
 * @since 1.0.0
 */
typedef enum
{
   PUD_OWNER_NOBODY                     = 0x03, /**< Noone controls the player */
   PUD_OWNER_PASSIVE_COMPUTER           = 0x02, /**< Player is a passive computer */
   PUD_OWNER_COMPUTER                   = 0x04, /**< Player is the computer (evil) */
   PUD_OWNER_HUMAN                      = 0x05, /**< Player is a human (not a computer) */
   PUD_OWNER_RESCUE_PASSIVE             = 0x06, /**< Player can be rescuable, but passive */
   PUD_OWNER_RESCUE_ACTIVE              = 0x07, /**< Player can be rescuable, and is active */
} Pud_Owner;

/**
 * @typedef Pud_Projectile
 * Types of projectiles known to Warcraft 2
 * @since 1.0.0
 */
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

/**
 * @type Pud_Allow
 * Type that holds permissions for a unit/upgrade/spell to be used,
 * researched by a HUMAN player (computers ignore this).
 * @since 1.0.0
 */
typedef uint32_t Pud_Allow;

/**
 * @typedef Pud_Side
 * Side of a player.
 * @since 1.0.0
 */
typedef enum
{
   PUD_SIDE_HUMAN                       = 0x00, /**< Human Race */
   PUD_SIDE_ORC                         = 0x01, /**< Orc Race */
   PUD_SIDE_NEUTRAL                     = 0x02, /**< Neutral (e.g. critters) */
} Pud_Side;

/**
 * @typedef Pud_Open_Mode
 * Cumulative flags that serves as arguments when opening a Pud file
 * @since 1.0.0
 */
typedef enum
{
   PUD_OPEN_MODE_R = (1 << 0), /**< Pud can be read */
   PUD_OPEN_MODE_W = (1 << 1), /**< Pud can be written */
   PUD_OPEN_MODE_RW = (PUD_OPEN_MODE_R | PUD_OPEN_MODE_W), /**< Pud can be read AND written */
   PUD_OPEN_MODE_NO_PARSE = (1 << 2), /**< Pud will not be parsed when opened */
} Pud_Open_Mode;

/**
 * @typedef Pud_Era
 * Type that holds the era of a map
 * @since 1.0.0
 */
typedef enum
{
   PUD_ERA_FOREST       = 0, /**< Forest */
   PUD_ERA_WINTER       = 1, /**< Winter (snow) */
   PUD_ERA_WASTELAND    = 2, /**< Wasteland */
   PUD_ERA_SWAMP        = 3  /**< Orc Swamp (Draenor) */
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


/*
 * Allow spells
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


/*
 * Allow upgrades
 */
#define PUD_ALLOW_UPGRADE_AXES_1                        ((uint_fast32_t)1 << (uint_fast32_t)0)
#define PUD_ALLOW_UPGRADE_AXES_2                        ((uint_fast32_t)1 << (uint_fast32_t)1)
#define PUD_ALLOW_UPGRADE_SWORDS_1                      ((uint_fast32_t)1 << (uint_fast32_t)2)
#define PUD_ALLOW_UPGRADE_SWORDS_2                      ((uint_fast32_t)1 << (uint_fast32_t)3)
#define PUD_ALLOW_UPGRADE_SHIELDS_1                     ((uint_fast32_t)1 << (uint_fast32_t)4)
#define PUD_ALLOW_UPGRADE_SHIELDS_2                     ((uint_fast32_t)1 << (uint_fast32_t)5)
#define PUD_ALLOW_UPGRADE_CANNONS_1                     ((uint_fast32_t)1 << (uint_fast32_t)6)
#define PUD_ALLOW_UPGRADE_CANNONS_2                     ((uint_fast32_t)1 << (uint_fast32_t)7)
#define PUD_ALLOW_UPGRADE_ARMOR_1                       ((uint_fast32_t)1 << (uint_fast32_t)8)
#define PUD_ALLOW_UPGRADE_ARMOR_2                       ((uint_fast32_t)1 << (uint_fast32_t)9)
/* Bit 10 is unused */
/* Bit 11 is unused */
#define PUD_ALLOW_UPGRADE_PROJECTILES_1                 ((uint_fast32_t)1 << (uint_fast32_t)12)
#define PUD_ALLOW_UPGRADE_PROJECTILES_2                 ((uint_fast32_t)1 << (uint_fast32_t)13)
/* Bit 14 is unused */
/* Bit 15 is unused */
#define PUD_ALLOW_UPGRADE_RANGERS_BERSERKERS            ((uint_fast32_t)1 << (uint_fast32_t)16)
#define PUD_ALLOW_UPGRADE_LONG_RANGE                    ((uint_fast32_t)1 << (uint_fast32_t)17)
#define PUD_ALLOW_UPGRADE_SCOUTING                      ((uint_fast32_t)1 << (uint_fast32_t)18)
#define PUD_ALLOW_UPGRADE_MARKSMANSHIP_REGENERATION     ((uint_fast32_t)1 << (uint_fast32_t)19)
/* Bits 20 to 31 are unused */


/**
 * @typedef Pud
 * Handle to manipulate a pud file.
 * Create a valid handle with pud_open()
 * @since 1.0.0
 */
typedef struct _Pud Pud;
typedef struct _Pud_Unit_Data Pud_Unit_Data;
typedef struct _Pud_Unit_Characteristics Pud_Unit_Characteristics;
typedef struct _Pud_Upgrade_Characteristics Pud_Upgrade_Characteristics;

/**
 * @typedef Pud_Private
 * Internal data of the Pud structure. It is not meant to be public.
 * Don't touch it, don't even think of accessing it.
 * If you really want to get into trouble, get the pud_private.h file
 * from the sources to access the internals.
 * @since 1.0.0
 */
typedef struct _Pud_Private Pud_Private;

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


/**
 * @struct _Pud
 * Public data of a Pud file's contents.
 * @since 1.0.0
 */
struct _Pud
{
   Pud_Private *private; /**< Internals, private and opaque data. Don't touch ;-) */

   uint32_t       tag;
   uint16_t       version;
   char           description[32];
   uint8_t        era;
   Pud_Dimensions dims;

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
   unsigned int starting_points;

   /* Number of elements is in 'tiles' */
   uint16_t *tiles_map;
   uint16_t *action_map;
   uint16_t *movement_map;
   uint8_t *oil_map;

   Pud_Unit_Data *units;

   unsigned int units_count;

   Pud_Unit_Characteristics unit_data[110]; /* [defaults] */

   uint16_t obsolete_udta[508];
};

/**
 * @typedef Pud_Color
 * Type that holds a 32-bits color
 * @since 1.0.0
 */
typedef struct _Pud_Color Pud_Color;

/**
 * @struct _Pud_Color
 * Public data for a 32-bits color.
 * @since 1.0.0
 */
struct _Pud_Color
{
   unsigned char r; /**< 8-bits red component */
   unsigned char g; /**< 8-bits green component */
   unsigned char b; /**< 8-bits blue component */
   unsigned char a; /**< 8-bits alpha component */
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

/**
 *
 * @defgroup Pud_General General Pud Functions
 *
 * General Pud functions can be used in any context, to query general
 * information about the PUD format, anything that is not related to
 * a specific PUD file.
 *
 * @{
 */

/**
 * Init the pud library.
 *
 * This function should be call once in the program
 * initialization. Calling a pud function without this call first may result
 * in undefined behaviour. Use pud_shutdown() to ensure potential resources
 * allocated by this function are properly released.
 *
 * @return #PUD_TRUE on success, #PUD_FALSE on failure
 * @note This function initializes the libc pseudo-random number generator.
 * @see pud_shutdown()
 * @since 1.0.0
 */
PUDAPI Pud_Bool pud_init(void);

/**
 * Shut the pud library down.
 *
 * Releases all resources allocated by pud_init().
 * It shall be called before terminating the program to ensure libpud internals
 * do not leak.
 *
 * @see pud_init()
 * @since 1.0.0
 */
PUDAPI void pud_shutdown(void);

/**
 * Determines if the PUD section is actually valid.
 *
 * @param sec The 4-bytes string section
 * @return #PUD_TRUE if valid, #PUD_FALSE otherwise
 * @since 1.0.0
 */
PUDAPI Pud_Bool pud_section_valid_is(const char sec[4]);

/**
 * Generates a random tag for a PUD map.
 *
 * Tags are used to uniquely identify a PUD file other a network (for
 * legacy multiplayers mode).
 *
 * @return A psuedo-random tag
 * @since 1.0.0
 */
PUDAPI uint32_t pud_tag_generate(void);

/**
 * Describe a Warcraft 2 unit
 *
 * @param unit The unit to be described
 * @param pretty If #PUD_FALSE, will return a compact identifier. If #PUD_FALSE,
 *               will return a complete string.
 * @return The description of @c unit according the @c pretty.
 * @since 1.0.0
 */
PUDAPI const char *pud_unit_to_string(Pud_Unit unit, Pud_Bool pretty);

/**
 * Describe a Pud era
 *
 * @param era The era to be described
 * @return The description of @c era
 * @since 1.0.0
 */
PUDAPI const char *pud_era_to_string(Pud_Era era);

/**
 * Describe a Pud color
 *
 * @param color The color to be described
 * @return The description of @c color
 * @since 1.0.0
 */
PUDAPI const char *pud_color_to_string(Pud_Player color);

/**
 * Describe a Warcraft 2 projectile
 *
 * @param proj The projectile to be described
 * @return The description of @c proj
 * @since 1.0.0
 */
PUDAPI const char *pud_projectile_to_string(Pud_Projectile proj);

/**
 * Describe a Warcraft 2 upgrade
 *
 * @param upgrade The upgrade to be described
 * @return The description of @c upgrade
 * @since 1.0.0
 */
PUDAPI const char *pud_upgrade_to_string(Pud_Upgrade upgrade);

/**
 * Get the icon associated to an upgrade
 *
 * @param upgrade The upgrade from which an icon will be queried
 * @return The icon associated to @c upgrade
 * @since 1.0.0
 */
PUDAPI Pud_Icon pud_upgrade_icon_get(Pud_Upgrade upgrade);

/**
 * Get the size, in cells, for a given Pud_Dimensions value
 *
 * @param dim A dimensions handler
 * @param w_ret Pointer to a valid storage zone for the width
 * @param h_ret Pointer to a valid storage zone for the height
 *
 * @note @c w_ret and @c h_ret will point to zero when @c dim is invalid
 * @since 1.0.0
 */
PUDAPI void pud_dimensions_to_size(Pud_Dimensions dim, unsigned int *w_ret, unsigned int *h_ret);

/**
 * Tell if the Pud_Allow Unit is valid
 *
 * @param flag The Pud_Allow combination of flags
 * @return #PUD_TRUE if valid, #PUD_FALSE otherwise
 * @since 1.0.0
 */
PUDAPI Pud_Bool pud_allow_unit_valid_is(Pud_Allow flag);

/**
 * Tell if the Pud_Allow Spell is valid
 *
 * @param flag The Pud_Allow combination of flags
 * @return #PUD_TRUE if valid, #PUD_FALSE otherwise
 * @since 1.0.0
 */
PUDAPI Pud_Bool pud_allow_spell_valid_is(Pud_Allow flag);

/**
 * Tell if the Pud_Allow Upgrade is valid
 *
 * @param flag The Pud_Allow combination of flags
 * @return #PUD_TRUE if valid, #PUD_FALSE otherwise
 * @since 1.0.0
 */
PUDAPI Pud_Bool pud_allow_upgrade_valid_is(Pud_Allow flag);

/**
 * Describe a Pud allowed unit as a string
 *
 * @param flag The allowed unit to be described
 * @return The description of @c flag
 * @since 1.0.0
 */
PUDAPI const char *pud_allow_unit_to_string(Pud_Allow flag);

/**
 * Describe a Pud allowed spell as a string
 *
 * @param flag The allowed spell to be described
 * @return The description of @c flag
 * @since 1.0.0
 */
PUDAPI const char *pud_allow_spell_to_string(Pud_Allow flag);

/**
 * Describe a Pud allowed upgrade as a string
 *
 * @param flag The allowed upgrade to be described
 * @return The description of @c flag
 * @since 1.0.0
 */
PUDAPI const char *pud_allow_upgrade_to_string(Pud_Allow flag);

/**
 * Get the icons associated to an allowed unit.
 *
 * Units are allowed by pair (Human & Orc counterparts). So getting the
 * icon for one also returns the icon of a unit counterpart in the opposite
 * race.
 *
 * @param flag The unit allowed
 * @return A pointer on a array of EXACTLY TWO icons. #NULL on failure
 * @since 1.0.0
 */
PUDAPI const Pud_Icon *pud_allow_unit_icons_get(Pud_Allow flag);

/**
 * Get the icon associated to an allowed spell.
 *
 * @param flag The spell allowed
 * @return The icon of the spell @c flag
 * @since 1.0.0
 */
PUDAPI Pud_Icon pud_allow_spell_icon_get(Pud_Allow flag);

/**
 * Get the icons associated to an allowed upgrade.
 *
 * Units are allowed by pair (Human & Orc counterparts). So getting the
 * icon for one also returns the icon of an upgrade counterpart in the opposite
 * race.
 *
 * @param flag The upgrade allowed
 * @return A pointer on a array of EXACTLY TWO icons. #NULL on failure
 * @since 1.0.0
 */
PUDAPI const Pud_Icon *pud_allow_upgrade_icons_get(Pud_Allow flag);

/**
 * Get the litteral name of a pud section
 * 
 * @param section A pud section
 * @return #NULL if @c section is invalid, its name otherwise
 */
PUDAPI const char *pud_section_to_string(Pud_Section section);


/**
 * @}
 */ /* End of group Pud_General */


/**
 * @defgroup Pud_File Pud File Group Functions
 *
 * Functions that allow to manipulate a Pud file.
 *
 * @{
 */

/**
 * Open a PUD file for later manipulation.
 *
 * A pud file can be opened read-only, read-write or write-only.
 * If the file is invalid and the read mode is chosen, this function will fail.
 * If the file is invalid, but only the write-only mode is chosen, a valid
 * handle will be returned.
 * Using the #PUD_OPEN_MODE_NO_PARSE with a read mode allows the file NOT to be
 * parsed. It must be handled by the developer manually, but with a finer
 * granularity. The default behaviour being parsing by default.
 *
 * @param file The path to the PUD file to open.
 * @param mode Give the access rights and possible other behaviours
 * @return #NULL on failure, a valid handler otherwise.
 * @see Pud_Open_Mode
 * @since 1.0.0
 */
PUDAPI Pud *pud_open(const char *file, Pud_Open_Mode mode);

/**
 * Close a previous opened PUD file
 *
 * Releases resources allocated when opening a PUD file.
 * @param pud A valid PUD handle
 * @see pud_open()
 * @since 1.0.0
 */
PUDAPI void pud_close(Pud *pud);

/**
 * Dump the internals of a valid Pud file into an IO stream
 *
 * @param A valid PUD handle
 * @param stream An IO stream. Defaults to #stdout is #NULL
 * @since 1.0.0
 */

PUDAPI void pud_dump(const Pud *pud, FILE *stream);

/**
 * Get the description of a Pud file.
 *
 * The description is a 32-bytes string. The description MUST
 * be NUL-terminated.
 *
 * @param A valid pud handle.
 * @return NULL if @c pud is NULL, a pointer to an array of 32 characters otherwise.
 * @since 1.0.0
 */
PUDAPI const char *pud_description_get(const Pud *pud);

/**
 * Set the description of a pud file
 *
 * The description is exactly 31 usable characters, plus a NUL terminator.
 * NUL-termination is enforcend, even when @c descr is not NUL-terminated.
 *
 * @param pud A valid pud handle.
 * @param descr The description to be set.
 * @since 1.0.0
 */
PUDAPI void pud_description_set(Pud *pud, const char descr[32]);

/**
 * Set the Warcraft 2 compatible version for the Pud
 *
 * @param pud A valid pud handle
 * @param version Warcraft 2 revision
 * @since 1.0.0
 */
PUDAPI void pud_version_set(Pud *pud, uint16_t version);

/**
 * Set the tag identifier of the Pud file
 *
 * Pud files are identified uniquely over a network thanks to this identifier.
 * A random tag can be generated with pud_tag_generate().
 *
 * @param pud A valid pud handle
 * @param tag The map identifier
 * @see pud_tag_generate()
 * @since 1.0.0
 */
PUDAPI void pud_tag_set(Pud *pud, uint32_t tag);

/**
 * Set the era of a Pud file
 *
 * @param pud A valid pud handle
 * @param era The era of the map
 * @since 1.0.0
 */
PUDAPI void pud_era_set(Pud *pud, Pud_Era era);

/**
 * Set and resize a Pud file
 *
 * This function sets the internal size of a Pud, and resize all allocated buffers
 * that rely on the map size. Resizing a map on the fly offers no guarantee on its
 * contents after the resize.
 *
 * @note Use this function ONLY when creating a pud from scratch!
 * @warning This function MUST NOT be used to resize an already existing map.
 *
 * @param pud A valid pud handle
 * @param dims The dimensions of the map
 * @return #PUD_TRUE on success, #PUD_FALSE on failure
 * @since 1.0.0
 */
PUDAPI Pud_Bool pud_dimensions_set(Pud *pud, Pud_Dimensions dims);

/**
 * Write the contents of a Pud in the filesystem
 *
 * No checks are performed. If the contents of @c pud are invalid,
 * the Pud file will still be written. The file will not be readable by Warcraft 2,
 * or will result in a crash if ill-formed.
 * To prevent this, run pud_check() before writing to make sure the Pud is
 * valid
 *
 * @param pud A valid pud handle
 * @param file The path where to write @c pud
 * @return #PUD_TRUE on success, #PUD_FALSE on failure.
 * @see pud_check()
 * @since 1.0.0
 */
PUDAPI Pud_Bool pud_write(const Pud *pud, const char *file);

/**
 * Verify a pud file integrity, and update its internal state to reflect
 * some changes.
 *
 * This function does basic checks on the Pud file, to ensure it is
 * playable and could be detected by Warcraft 2. It may rewrite internal
 * contents of @c pud, because some internal fields require a whole pud
 * rechecking to be updated.
 *
 * It is mandatory to call this function before writing to the filesystem
 * with pud_write(). Some query operations might also return invalid results
 * after a Pud modification if this function is not called before.
 *
 * When an error is detected, it returns its code and stores in memory pointed
 * by @c err (if not NULL) a descriptive structure that allows debugging.
 *
 * @param pud A valid pud handle
 * @param err A pointer to the error reason. Ignored if #NULL
 * @return An error code. #PUD_ERROR_NONE if everything is fine.
 * @since 1.0.0
 */
PUDAPI Pud_Error pud_check(Pud *pud, Pud_Error_Description *err);

/**
 * Set the tile in a given cell
 *
 * This is a convenient function to write a tile in a given cell.
 *
 * @note If you need high performances, consider NOT using this function, as it
 * does safety checks to ensure proper memory access.
 *
 * @param pud A valid pud handle
 * @param x The X coordinate of the cell
 * @param y The Y coordinate of the cell
 * @param tile The tile to set
 * @return #PUD_TRUE on success, #PUD_FALSE on failure
 * @since 1.0.0
 */
PUDAPI Pud_Bool pud_tile_set(Pud *pud, unsigned int x, unsigned int y, uint16_t tile);

/**
 * Get the tile in a given cell
 *
 * @param pud A valid pud handle
 * @param x The X coordinate of the cell
 * @param y The Y coordinate of the cell
 * @return The tile at @c x @c y. 0x0000 on failure.
 * @since 1.0.0
 */
PUDAPI uint16_t pud_tile_get(const Pud *pud, unsigned int x, unsigned int y);

/**
 * Add a unit in the Pud
 *
 * The top-left coordinate of the unit will be placed at @c x, @c y.
 * Make sure that the unit does not get out of bounds!
 *
 * @param pud A valid pud handle
 * @param x The X coordinate of the cell
 * @param y The Y coordinate of the cell
 * @param owner The owner of the unit
 * @param unit The unit to add
 * @param alter This value depends on the unit type. Refer to the Pud format
 * documentation for more details.
 * @return #PUD_TRUE on success, #PUD_FALSE on failure
 * @since 1.0.0
 */
PUDAPI Pud_Bool pud_unit_add(Pud *pud, unsigned int x, unsigned int y, Pud_Player owner, Pud_Unit unit, uint16_t alter);

/**
 * Get the side of a player in a given Pud file
 *
 * @param pud a valid pud handle
 * @param player The player from which the side is queried.
 * @return The side (race) of @c player
 * @since 1.0.0
 */
PUDAPI Pud_Side pud_side_for_player_get(const Pud *pud, Pud_Player player);

/**
 * Write defaults ALOW values in a pud file
 *
 * Pud files do not ship the ALOW section, because it is optional.
 * When writing new ALOW from an editor (i.e. war2edit), we need to write
 * a default ALOW section.
 *
 * @param pud A valid pud handle
 * @since 1.0.0
 */
PUDAPI void pud_alow_defaults_set(Pud *pud);

/**
 * Generate a bitmap of the minimap for a given Pud
 *
 * @note The returned memory is malloc()ed, and is NOT managed by libpud.
 * It is up to the caller to this function to use free() on the returned
 * bitmap to release the memory.
 *
 * @param pud A valid pud handle
 * @param size_ret Stores the size of the bitmap. Ignored if #NULL.
 * @param pfmt The pixel format of the bitmap.
 * @return The bitmap holding the minimap. #NULL on failure.
 * @since 1.0.0
 */
PUDAPI unsigned char *pud_minimap_bitmap_generate(const Pud *pud, unsigned int *size_ret, Pud_Pixel_Format pfmt);

/**
 * Write defaults UDTA, UGRD, ALOW, SGLD, SLBR, SOIL, AIPL, SIDE, OWNR.
 *
 * @param pud A valid pud handle
 * @return #PUD_TRUE on success, #PUD_FALSE otherwise
 * @since 1.0.0
 */
PUDAPI Pud_Bool pud_defaults_set(Pud *pud);

/**
 * Determine whether a Pud file has a section
 *
 * @param pud A valid pud handle
 * @param section The section to query
 * @return #PUD_TRUE if @c pud has @c section, #PUD_FALSE otherwise
 */
PUDAPI Pud_Bool pud_section_has(const Pud *pud, Pud_Section section);

/**
 * @}
 */ /* End of Pud_File group */

PUDAPI Pud_Owner pud_owner_convert(uint8_t code);
PUDAPI Pud_Side pud_side_convert(uint8_t code);



PUDAPI Pud_Bool pud_unit_building_is(Pud_Unit unit);

PUDAPI Pud_Bool pud_unit_start_location_is(Pud_Unit unit);
PUDAPI Pud_Bool pud_unit_flying_is(Pud_Unit unit);
PUDAPI Pud_Bool pud_unit_underwater_is(Pud_Unit unit);
PUDAPI Pud_Bool pud_unit_land_is(Pud_Unit unit);
PUDAPI Pud_Bool pud_unit_marine_is(Pud_Unit unit);
PUDAPI Pud_Bool pud_unit_always_passive_is(Pud_Unit unit);
PUDAPI Pud_Bool pud_unit_oil_well_is(Pud_Unit unit);
PUDAPI Pud_Bool pud_unit_coast_building_is(Pud_Unit unit);
PUDAPI Pud_Bool pud_unit_boat_is(Pud_Unit unit);

PUDAPI Pud_Bool pud_unit_hero_is(Pud_Unit unit);

PUDAPI Pud_Color pud_tile_to_color(Pud_Era era, uint16_t tile);
PUDAPI Pud_Color pud_color_for_player(Pud_Player player);
PUDAPI Pud_Color pud_gold_mine_color_get(void);
PUDAPI Pud_Color pud_oil_patch_color_get(void);
PUDAPI Pud_Color pud_color_for_unit(Pud_Unit unit, Pud_Player player);

PUDAPI Pud_Side pud_unit_side_get(Pud_Unit unit);

PUDAPI uint8_t pud_random_get(uint16_t tile);
PUDAPI Pud_Icon pud_unit_icon_get(Pud_Unit unit);
PUDAPI Pud_Bool pud_unit_valid_is(Pud_Unit unit);
PUDAPI Pud_Unit pud_unit_switch_side(Pud_Unit unit);

/**
 * @defgroup Pud_Parse Pud Advanced Parsing Functions
 *
 * Parsing functions are provided for a user to have fine granularity over
 * parsing. These functions are automatically used by default internally
 * when opening a file, unless otherwise specified.
 *
 * pud_parse() parses all the possible sections. You can parse individual
 * sections with the appropriate setions, but be aware that parsing one
 * section might require parsing of a another one! Please read the PUD
 * format documentation for more details.
 *
 * @{
 */

/**
 * Parse a previously opened Pud file.
 *
 * By default, when opening a file with the #PUD_OPEN_MODE_R flag the pud file
 * is automatically parsed, unless #PUD_OPEN_MODE_NO_PARSE is specified.
 *
 * @param A valid Pud handle
 * @return #PUD_TRUE on success, #PUD_FALSE otherwise
 * @see pud_open()
 * @see Pud_Open_Mode
 */
PUDAPI Pud_Bool pud_parse(Pud *pud);

/**
 * Parse the TYPE section.
 *
 * @param pud a valid Pud handle
 * @return #PUD_TRUE on success, #PUD_FALSE on failure
 */
PUDAPI Pud_Bool pud_parse_type(Pud *pud);

/**
 * Parse the VER  section.
 *
 * @param pud a valid Pud handle
 * @return #PUD_TRUE on success, #PUD_FALSE on failure
 */
PUDAPI Pud_Bool pud_parse_ver(Pud *pud);

/**
 * Parse the DESC section.
 *
 * @param pud a valid Pud handle
 * @return #PUD_TRUE on success, #PUD_FALSE on failure
 */
PUDAPI Pud_Bool pud_parse_desc(Pud *pud);

/**
 * Parse the OWNR section.
 *
 * @param pud a valid Pud handle
 * @return #PUD_TRUE on success, #PUD_FALSE on failure
 */
PUDAPI Pud_Bool pud_parse_ownr(Pud *pud);

/**
 * Parse the SIDE section.
 *
 * @param pud a valid Pud handle
 * @return #PUD_TRUE on success, #PUD_FALSE on failure
 */
PUDAPI Pud_Bool pud_parse_side(Pud *pud);

/**
 * Parse the ERA  and ERAX sections.
 *
 * @param pud a valid Pud handle
 * @return #PUD_TRUE on success, #PUD_FALSE on failure
 */
PUDAPI Pud_Bool pud_parse_era(Pud *pud);

/**
 * Parse the DIM  section.
 *
 * @param pud a valid Pud handle
 * @return #PUD_TRUE on success, #PUD_FALSE on failure
 */
PUDAPI Pud_Bool pud_parse_dim(Pud *pud);

/**
 * Parse the UDTA section.
 *
 * @param pud a valid Pud handle
 * @return #PUD_TRUE on success, #PUD_FALSE on failure
 */
PUDAPI Pud_Bool pud_parse_udta(Pud *pud);

/**
 * Parse the ALOW section.
 *
 * @param pud a valid Pud handle
 * @return #PUD_TRUE on success, #PUD_FALSE on failure
 */
PUDAPI Pud_Bool pud_parse_alow(Pud *pud);

/**
 * Parse the UGRD section.
 *
 * @param pud a valid Pud handle
 * @return #PUD_TRUE on success, #PUD_FALSE on failure
 */
PUDAPI Pud_Bool pud_parse_ugrd(Pud *pud);

/**
 * Parse the SGLD section.
 *
 * @param pud a valid Pud handle
 * @return #PUD_TRUE on success, #PUD_FALSE on failure
 */
PUDAPI Pud_Bool pud_parse_sgld(Pud *pud);

/**
 * Parse the SLBR section.
 *
 * @param pud a valid Pud handle
 * @return #PUD_TRUE on success, #PUD_FALSE on failure
 */
PUDAPI Pud_Bool pud_parse_slbr(Pud *pud);

/**
 * Parse the SOIL section.
 *
 * @param pud a valid Pud handle
 * @return #PUD_TRUE on success, #PUD_FALSE on failure
 */
PUDAPI Pud_Bool pud_parse_soil(Pud *pud);

/**
 * Parse the AIPL section.
 *
 * @param pud a valid Pud handle
 * @return #PUD_TRUE on success, #PUD_FALSE on failure
 */
PUDAPI Pud_Bool pud_parse_aipl(Pud *pud);

/**
 * Parse the MTXM section.
 *
 * @param pud a valid Pud handle
 * @return #PUD_TRUE on success, #PUD_FALSE on failure
 */
PUDAPI Pud_Bool pud_parse_mtxm(Pud *pud);

/**
 * Parse the SQM  section.
 *
 * @param pud a valid Pud handle
 * @return #PUD_TRUE on success, #PUD_FALSE on failure
 */
PUDAPI Pud_Bool pud_parse_sqm(Pud *pud);

/**
 * Parse the OILM section.
 *
 * @param pud a valid Pud handle
 * @return #PUD_TRUE on success, #PUD_FALSE on failure
 */
PUDAPI Pud_Bool pud_parse_oilm(Pud *pud);

/**
 * Parse the REGM section.
 *
 * @param pud a valid Pud handle
 * @return #PUD_TRUE on success, #PUD_FALSE on failure
 */
PUDAPI Pud_Bool pud_parse_regm(Pud *pud);

/**
 * Parse the UNIT section.
 *
 * @param pud a valid Pud handle
 * @return #PUD_TRUE on success, #PUD_FALSE on failure
 */
PUDAPI Pud_Bool pud_parse_unit(Pud *pud);

/**
 * @}
 */ /* End of Pud_Parse group */



#ifdef __cplusplus
}
#endif
#endif /* ! __LIBPUD_PUD_H__ */
