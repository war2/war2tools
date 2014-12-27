#ifndef _PUD_H_
#define _PUD_H_
#ifdef __cplusplus
extern "C" <%
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bool.h"
#include "debug.h"

typedef enum
{
   PUD_SECTION_TYPE     = 0,
   PUD_SECTION_VER      = 1,
   PUD_SECTION_DESC     = 2,
   PUD_SECTION_OWNR     = 3,
   PUD_SECTION_ERA      = 4,
   PUD_SECTION_ERAX     = 5,
   PUD_SECTION_DIM      = 6,
   PUD_SECTION_UDTA     = 7,
   PUD_SECTION_ALOW     = 8,
   PUD_SECTION_UGRD     = 9,
   PUD_SECTION_SIDE     = 10,
   PUD_SECTION_SGLD     = 11,
   PUD_SECTION_SLBR     = 12,
   PUD_SECTION_SOIL     = 13,
   PUD_SECTION_AIPL     = 14,
   PUD_SECTION_MTXM     = 15,
   PUD_SECTION_SQM      = 16,
   PUD_SECTION_OILM     = 17,
   PUD_SECTION_REGM     = 18,
   PUD_SECTION_UNIT     = 19
} Pud_Section;

typedef enum
{
   PUD_DIMENSIONS_UNDEFINED = 0,
   PUD_DIMENSIONS_32_32,
   PUD_DIMENSIONS_64_64,
   PUD_DIMENSIONS_96_96,
   PUD_DIMENSIONS_128_128
} Pud_Dimensions;

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
   PUD_UNIT_INFANTRY                    = 0x00,
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
   PUD_UNIT_KURDAN_AND_SKY_REE          = 0x16,
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
   PUD_UNIT_KHORGATH_BLADEFIST          = 0x2f,
   PUD_UNIT_CHO_GALL                    = 0x31,
   PUD_UNIT_LOTHAR                      = 0x32,
   PUD_UNIT_GUL_DAN                     = 0x33,
   PUD_UNIT_UTHER_LIGHTBRINGER          = 0x34,
   PUD_UNIT_ZULJIN                      = 0x35,
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
   PUD_UNIT_ORC_WALL                    = 0x68
} Pud_Unit;

typedef enum
{
   PUD_OWNER_NOBODY                     = 0x03,
   PUD_OWNER_PASSIVE_COMPUTER           = 0x02,
   PUD_OWNER_COMPUTER                   = 0x01,
   PUD_OWNER_HUMAN                      = 0x05,
   PUD_OWNER_RESCUE_PASSIVE             = 0x06,
   PUD_OWNER_RESCUE_ACTIVE              = 0x07
} Pud_Owner;

typedef enum
{
   PUD_SIDE_HUMAN                       = 0x00,
   PUD_SIDE_ORC                         = 0x01,
   PUD_SIDE_NEUTRAL                     = 0x02
} Pud_Side;

typedef enum
{
   PUD_OPEN_MODE_R   = 1, /* (1 << 0) */
   PUD_OPEN_MODE_W   = 2, /* (1 << 1) */
   PUD_OPEN_MODE_RW  = 3  /* (1 << 0) | (1 << 1) */
} Pud_Open_Mode;

typedef enum
{
   PUD_ERA_FOREST,
   PUD_ERA_WINTER,
   PUD_ERA_WASTELAND,
   PUD_ERA_SWAMP
} Pud_Era;

typedef struct _Pud Pud;

struct _Pud
{
   FILE         *file;
   Pud_Open_Mode open_mode;

   uint32_t      tag;
   uint16_t      version;
   char          description[32];
   uint8_t       era;
   Pud_Dimensions dims;

   struct {
      uint8_t players[8];
      uint8_t unusable[7];
      uint8_t neutral;
   } owner;

   struct {
      uint8_t players[8];
      uint8_t unusable[7];
      uint8_t neutral;
   } side;

   struct {
      uint16_t players[8];
      uint16_t unusable[7];
      uint16_t neutral;
   } sgld, slbr, soil;

   struct {
      uint8_t players[8];
      uint8_t unusable[7];
      uint8_t neutral;
   } ai;

   struct _alow {
      uint32_t players[8];
      uint32_t unusable[7];
      uint32_t neutral;
   } unit_alow, spell_start, spell_alow, spell_acq, up_alow, up_acq;

   struct {
      uint8_t           time;
      uint16_t          gold;
      uint16_t          lumber;
      uint16_t          oil;
      uint16_t          icon;
      uint16_t          group;
      uint32_t          flags;
   } upgrade[52];

   int map_w;
   int map_h;

   uint16_t *tiles_map;
   uint16_t *action_map;
   uint16_t *movement_map;

   int tiles; /* pud->map_w * pud->map_h */

   struct _unit {
      uint16_t x;
      uint16_t y;
      uint8_t  type;
      uint8_t  owner;
      uint16_t alter;
   } *units;

   int units_count;

   struct {
      uint16_t     overlap_frames;
      uint32_t     sight;
      uint16_t     hp;
      uint8_t      build_time;
      uint8_t      gold_cost;
      uint8_t      lumber_cost;
      uint8_t      oil_cost;
      uint16_t     size_w;
      uint16_t     size_h;
      uint16_t     box_w;
      uint16_t     box_h;
      uint8_t      range;
      uint8_t      computer_react_range;
      uint8_t      human_react_range;
      uint8_t      armor;
      uint8_t      priority;
      uint8_t      basic_damage;
      uint8_t      piercing_damage;
      uint8_t      missile_weapon;
      uint8_t      type;
      uint8_t      decay_rate;
      uint8_t      annoy;
      uint8_t      mouse_right_btn;
      uint16_t     point_value;
      uint8_t      can_target;
      uint32_t     flags;
      unsigned int rect_sel           : 1;
      unsigned int has_magic          : 1;
      unsigned int weapons_upgradable : 1;
      unsigned int armor_upgradable   : 1;
   } unit_data[110];

   Pud_Section   current_section;

   unsigned int  verbose       : 4;
   unsigned int  default_allow : 1;
   unsigned int  default_udta  : 1;
   unsigned int  default_ugrd  : 1;
};



Pud *pud_open(const char *file, Pud_Open_Mode mode);
void pud_close(Pud *pud);
bool pud_parse(Pud *pud);
void pud_verbose_set(Pud *pud, int lvl);
bool pud_section_is_optional(Pud_Section sec);
uint32_t pud_go_to_section(Pud *pud, Pud_Section sec);
void pud_print(Pud *pud, FILE *stream);
void pud_dimensions_to_size(Pud_Dimensions dim, int *x_ret, int *y_ret);
Pud_Owner pud_owner_convert(uint8_t code);
Pud_Side pud_side_convert(uint8_t code);

bool pud_defaults_set(Pud *pud);

bool pud_parse_type(Pud *pud);
bool pud_parse_ver(Pud *pud);
bool pud_parse_desc(Pud *pud);
bool pud_parse_ownr(Pud *pud);
bool pud_parse_side(Pud *pud);
bool pud_parse_era(Pud *pud);
bool pud_parse_dim(Pud *pud);
bool pud_parse_udta(Pud *pud);
bool pud_parse_alow(Pud *pud);
bool pud_parse_ugrd(Pud *pud);
bool pud_parse_sgld(Pud *pud);
bool pud_parse_slbr(Pud *pud);
bool pud_parse_soil(Pud *pud);
bool pud_parse_aipl(Pud *pud);
bool pud_parse_mtxm(Pud *pud);
bool pud_parse_sqm(Pud *pud);
bool pud_parse_oilm(Pud *pud);
bool pud_parse_regm(Pud *pud);
bool pud_parse_unit(Pud *pud);

bool pud_minimap_to_ppm(Pud *pud, const char *file);
bool pud_minimap_to_jpeg(Pud *pud, const char *file);

uint16_t pud_tile_at(Pud *pud, int x, int y);

#ifdef __cplusplus
%>
#endif
#endif /* ! _PUD_H_ */
