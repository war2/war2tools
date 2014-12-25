#include "pudviewer.h"

#define PUD_VERBOSE(pud, msg, ...) \
   do { \
      if (pud->verbose) fprintf(stdout, "-- " msg "\n", ## __VA_ARGS__); \
   } while (0)

#define PUD_SANITY_CHECK(pud, ...) \
   do { \
      if (!(pud) || !((pud)->file)) { \
         DIE_RETURN(__VA_ARGS__, "Invalid PUD input [%p]", pud); \
      } \
   } while (0)

#define PUD_CHECK_FERROR(f, ret) \
   do { \
      if (ferror(f)) DIE_RETURN(ret, "Error while reading file"); \
   } while (0)


typedef struct _Color Color;

struct _Color
{
   unsigned char r;
   unsigned char g;
   unsigned char b;
};


struct _Pud
{
   FILE         *file;

   uint32_t      tag;
   uint16_t      version;
   char          description[32];
   uint8_t       era;
   Pud_Dimensions dims;

   struct {
      uint16_t players[8];
      uint16_t unusable[7];
      uint16_t neutral;
   } sgld, slbr, soil;

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
   } upgrade_cost[52];

   int map_w;
   int map_h;

   uint16_t *map_tiles;

   int map_tiles_count;

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

   unsigned int  verbose       : 1;
   unsigned int  default_allow : 1;
   unsigned int  default_udta  : 1;
};

static const char * const _sections[] =
{
   "TYPE", "VER ", "DESC", "OWNR", "ERA ",
   "ERAX", "DIM ", "UDTA", "ALOW", "UGRD",
   "SIDE", "SGLD", "SLBR", "SOIL", "AIPL",
   "MTXM", "SQM ", "OILM", "REGM", "UNIT"
};

static Color
_color_make(uint8_t r,
            uint8_t g,
            uint8_t b)
{
   Color c = {
      .r = r,
      .g = g,
      .b = b
   };
   return c;
}

static Color
_color_for_player(uint8_t player)
{
   switch (player)
     {
      case 0: return _color_make(0xc0, 0x00, 0x00); // Red
      case 1: return _color_make(0x00, 0x00, 0xc0); // Blue
      case 2: return _color_make(0x00, 0xff, 0x00); // Green
      case 3: return _color_make(0x80, 0x00, 0xc0); // Violet
      case 4: return _color_make(0xff, 0x80, 0x00); // Orange
      case 5: return _color_make(0x00, 0x00, 0x00); // Black
      case 6: return _color_make(0xff, 0xff, 0xff); // White
      case 7: return _color_make(0xff, 0xff, 0x00); // Yellow

      default:
              break;
     }

   return _color_make(0x7f, 0x7f, 0x7f);
}

static Color
_pud_tile_to_color(Pud      *pud,
                   uint16_t  tile)
{
   /* FIXME This is just for SUMMER
    * TODO WINTER
    * TODO WASTELAND
    * TODO SWAMP
    */
   switch (tile)
     {
      case 0x006a: return _color_make(0x24, 0x48, 0x04);
      case 0x006f: return _color_make(0x48, 0x48, 0x48);
      case 0x0060: return _color_make(0x24, 0x48, 0x04);
      case 0x0064: return _color_make(0x24, 0x48, 0x04);
      case 0x0640: return _color_make(0x28, 0x54, 0x0c);
      case 0x0790: return _color_make(0x14, 0x34, 0x00);
      case 0x0070: return _color_make(0x00, 0x4c, 0x00);
      case 0x0071: return _color_make(0x00, 0x4c, 0x00);
      case 0x0072: return _color_make(0x00, 0x4c, 0x00);
      case 0x0741: return _color_make(0x00, 0x4c, 0x00);
      case 0x0571: return _color_make(0x28, 0x54, 0x0c);
      case 0x05d1: return _color_make(0x28, 0x54, 0x0c);
      case 0x0470: return _color_make(0x6c, 0x40, 0x00);
      case 0x04d0: return _color_make(0x3c, 0x3c, 0x3c);
      case 0x0083: return _color_make(0x3c, 0x3c, 0x3c);
      case 0x0082: return _color_make(0x3c, 0x3c, 0x3c);
      case 0x0440: return _color_make(0x30, 0x30, 0x30);
      case 0x0560: return _color_make(0x6c, 0x40, 0x00);
      case 0x0501: return _color_make(0x2c, 0x5c, 0x10);
      case 0x0055: return _color_make(0x28, 0x54, 0x0c);
      case 0x0771: return _color_make(0x28, 0x54, 0x0c);
      case 0x07d0: return _color_make(0x40, 0x2c, 0x00);
      case 0x0760: return _color_make(0x00, 0x4c, 0x00);
      case 0x0720: return _color_make(0x00, 0x2c, 0x00);
      case 0x0701: return _color_make(0x00, 0x2c, 0x00);
      case 0x0054: return _color_make(0x24, 0x48, 0x04);
      case 0x0570: return _color_make(0x2c, 0x5c, 0x10);
      case 0x05d0: return _color_make(0x50, 0x30, 0x00);
      case 0x0290: return _color_make(0x74, 0x44, 0x04);
      case 0x0260: return _color_make(0x04, 0x34, 0x70);
      case 0x0220: return _color_make(0x04, 0x34, 0x70);
      case 0x0222: return _color_make(0x00, 0x28, 0x5c);
      case 0x0221: return _color_make(0x00, 0x30, 0x68);
      case 0x0281: return _color_make(0x00, 0x30, 0x68);
      case 0x02b1: return _color_make(0x6c, 0x40, 0x00);
      case 0x02b2: return _color_make(0x74, 0x44, 0x04);
      case 0x0230: return _color_make(0x74, 0x44, 0x04);
      case 0x05c0: return _color_make(0x74, 0x44, 0x04);
      case 0x0531: return _color_make(0x2c, 0x5c, 0x10);
      case 0x0056: return _color_make(0x24, 0x48, 0x04);
      case 0x005e: return _color_make(0x24, 0x48, 0x04);
      case 0x0271: return _color_make(0x6c, 0x40, 0x00);
      case 0x02d1: return _color_make(0x6c, 0x40, 0x00);
      case 0x0011: return _color_make(0x04, 0x38, 0x74);
      case 0x0012: return _color_make(0x04, 0x38, 0x74);
      case 0x0010: return _color_make(0x04, 0x38, 0x74);
      case 0x0013: return _color_make(0x04, 0x38, 0x74);
      case 0x02c1: return _color_make(0x00, 0x30, 0x68);
      case 0x0039: return _color_make(0x60, 0x38, 0x00);
      case 0x0037: return _color_make(0x6c, 0x40, 0x00);
      case 0x05c1: return _color_make(0x6c, 0x40, 0x00);
      case 0x0530: return _color_make(0x4c, 0x6c, 0x1c);
      case 0x0061: return _color_make(0x24, 0x48, 0x04);
      case 0x0062: return _color_make(0x24, 0x48, 0x04);
      case 0x0661: return _color_make(0x28, 0x54, 0x0c);
      case 0x0601: return _color_make(0x28, 0x54, 0x0c);
      case 0x0711: return _color_make(0x00, 0x2c, 0x00);
      case 0x0721: return _color_make(0x00, 0x4c, 0x00);
      case 0x07a1: return _color_make(0x00, 0x4c, 0x00);
      case 0x0591: return _color_make(0x2c, 0x5c, 0x10);
      case 0x0471: return _color_make(0x6c, 0x40, 0x00);
      case 0x0081: return _color_make(0x48, 0x48, 0x48);
      case 0x0080: return _color_make(0x18, 0x18, 0x18);
      case 0x0441: return _color_make(0x3c, 0x3c, 0x3c);
      case 0x0540: return _color_make(0x74, 0x44, 0x04);
      case 0x07b0: return _color_make(0x28, 0x54, 0x0c);
      case 0x0730: return _color_make(0x28, 0x54, 0x0c);
      case 0x0761: return _color_make(0x00, 0x4c, 0x00);
      case 0x07a0: return _color_make(0x00, 0x4c, 0x00);
      case 0x05b2: return _color_make(0x28, 0x54, 0x0c);
      case 0x0270: return _color_make(0x6c, 0x40, 0x00);
      case 0x0250: return _color_make(0x60, 0x38, 0x00);
      case 0x0200: return _color_make(0x60, 0x38, 0x00);
      case 0x0521: return _color_make(0x74, 0x44, 0x04);
      case 0x05a1: return _color_make(0x6c, 0x40, 0x00);
      case 0x0210: return _color_make(0x74, 0x44, 0x04);
      case 0x02a1: return _color_make(0x60, 0x38, 0x00);
      case 0x02c0: return _color_make(0x00, 0x30, 0x68);
      case 0x02b0: return _color_make(0x6c, 0x40, 0x00);
      case 0x0231: return _color_make(0x6c, 0x40, 0x00);
      case 0x06c0: return _color_make(0x28, 0x54, 0x0c);
      case 0x06b0: return _color_make(0x28, 0x54, 0x0c);
      case 0x0631: return _color_make(0x2c, 0x5c, 0x10);
      case 0x0052: return _color_make(0x28, 0x54, 0x0c);
      case 0x0710: return _color_make(0x14, 0x34, 0x00);
      case 0x05b0: return _color_make(0x50, 0x30, 0x00);
      case 0x0491: return _color_make(0x30, 0x30, 0x30);
      case 0x0541: return _color_make(0x60, 0x38, 0x00);
      case 0x07c1: return _color_make(0x00, 0x4c, 0x00);
      case 0x0700: return _color_make(0x00, 0x2c, 0x00);
      case 0x0740: return _color_make(0x00, 0x4c, 0x00);
      case 0x0251: return _color_make(0x60, 0x38, 0x00);
      case 0x0201: return _color_make(0x00, 0x20, 0x54);
      case 0x0500: return _color_make(0x74, 0x44, 0x04);
      case 0x0511: return _color_make(0x24, 0x48, 0x04);
      case 0x0522: return _color_make(0x6c, 0x40, 0x00);
      case 0x0520: return _color_make(0x6c, 0x40, 0x00);
      case 0x0211: return _color_make(0x6c, 0x40, 0x00);
      case 0x0280: return _color_make(0x00, 0x30, 0x68);
      case 0x05b1: return _color_make(0x2c, 0x5c, 0x10);
      case 0x02a0: return _color_make(0x48, 0x28, 0x08);
      case 0x0261: return _color_make(0x04, 0x38, 0x74);
      case 0x0240: return _color_make(0x00, 0x20, 0x54);
      case 0x06c1: return _color_make(0x28, 0x54, 0x0c);
      case 0x06b2: return _color_make(0x2c, 0x5c, 0x10);
      case 0x0050: return _color_make(0x28, 0x54, 0x0c);
      case 0x04b0: return _color_make(0x6c, 0x40, 0x00);
      case 0x005f: return _color_make(0x28, 0x54, 0x0c);
      case 0x0592: return _color_make(0x2c, 0x5c, 0x10);
      case 0x0510: return _color_make(0x28, 0x54, 0x0c);
      case 0x005c: return _color_make(0x24, 0x48, 0x04);
      case 0x05a0: return _color_make(0x6c, 0x40, 0x00);
      case 0x06b1: return _color_make(0x28, 0x54, 0x0c);
      case 0x0670: return _color_make(0x2c, 0x5c, 0x10);
      case 0x0791: return _color_make(0x14, 0x34, 0x00);
      case 0x0770: return _color_make(0x24, 0x48, 0x04);
      case 0x07b1: return _color_make(0x28, 0x54, 0x0c);
      case 0x07d1: return _color_make(0x40, 0x2c, 0x00);
      case 0x0242: return _color_make(0x00, 0x20, 0x54);
      case 0x0561: return _color_make(0x6c, 0x40, 0x00);
      case 0x0731: return _color_make(0x28, 0x54, 0x0c);
      case 0x02d0: return _color_make(0x60, 0x38, 0x00);
      case 0x0051: return _color_make(0x28, 0x54, 0x0c);
      case 0x0690: return _color_make(0x2c, 0x5c, 0x10);
      case 0x0642: return _color_make(0x28, 0x54, 0x0c);
      case 0x0460: return _color_make(0x74, 0x74, 0x74);
      case 0x0420: return _color_make(0x7c, 0x7c, 0x7c);
      case 0x0400: return _color_make(0x50, 0x50, 0x50);
      case 0x0590: return _color_make(0x28, 0x54, 0x0c);
      case 0x0542: return _color_make(0x3c, 0x64, 0x14);
      case 0x0630: return _color_make(0x2c, 0x5c, 0x10);
      case 0x0671: return _color_make(0x2c, 0x5c, 0x10);
      case 0x06d0: return _color_make(0x2c, 0x5c, 0x10);
      case 0x0600: return _color_make(0x28, 0x54, 0x0c);
      case 0x0490: return _color_make(0x48, 0x48, 0x48);
      case 0x0401: return _color_make(0x7c, 0x7c, 0x7c);
      case 0x0660: return _color_make(0x28, 0x54, 0x0c);
      case 0x07c0: return _color_make(0x00, 0x4c, 0x00);
      case 0x0292: return _color_make(0x74, 0x44, 0x04);
      case 0x0641: return _color_make(0x28, 0x54, 0x0c);
      case 0x0030: return _color_make(0x6c, 0x40, 0x00);
      case 0x005d: return _color_make(0x28, 0x54, 0x0c);
      case 0x0031: return _color_make(0x74, 0x44, 0x04);
      case 0x0421: return _color_make(0x68, 0x68, 0x68);
      case 0x04a0: return _color_make(0x3c, 0x3c, 0x3c);
      case 0x04c0: return _color_make(0x3c, 0x3c, 0x3c);
      case 0x0431: return _color_make(0x68, 0x68, 0x68);
      case 0x005b: return _color_make(0x28, 0x54, 0x0c);
      case 0x0411: return _color_make(0x74, 0x74, 0x74);
      case 0x0691: return _color_make(0x2c, 0x5c, 0x10);
      case 0x006c: return _color_make(0xff, 0xff, 0x00);
      case 0x0241: return _color_make(0x00, 0x30, 0x68);
      case 0x0610: return _color_make(0x2c, 0x5c, 0x10);
      case 0x0620: return _color_make(0x28, 0x54, 0x0c);
      case 0x0066: return _color_make(0x24, 0x48, 0x04);
      case 0x0067: return _color_make(0x24, 0x48, 0x04);
      case 0x0291: return _color_make(0x74, 0x44, 0x04);
      case 0x04b1: return _color_make(0x6c, 0x40, 0x00);
      case 0x0430: return _color_make(0x60, 0x38, 0x00);
      case 0x06a1: return _color_make(0x28, 0x54, 0x0c);
      case 0x06a0: return _color_make(0x28, 0x54, 0x0c);
      case 0x0611: return _color_make(0x28, 0x54, 0x0c);
      case 0x0410: return _color_make(0x3c, 0x3c, 0x3c);
      case 0x0621: return _color_make(0x28, 0x54, 0x0c);
      case 0x0622: return _color_make(0x28, 0x54, 0x0c);
      case 0x005a: return _color_make(0x24, 0x48, 0x04);
      case 0x0751: return _color_make(0x00, 0x2c, 0x00);
      case 0x0058: return _color_make(0x24, 0x48, 0x04);
      case 0x0780: return _color_make(0x00, 0x2c, 0x00);
      case 0x0057: return _color_make(0x2c, 0x5c, 0x10);
      case 0x006b: return _color_make(0x48, 0x48, 0x48);
      case 0x0692: return _color_make(0x2c, 0x5c, 0x10);
      case 0x06d1: return _color_make(0x2c, 0x5c, 0x10);
      case 0x0750: return _color_make(0x00, 0x2c, 0x00);
      case 0x0370: return _color_make(0x6c, 0x40, 0x00);
      case 0x0331: return _color_make(0x74, 0x44, 0x04);
      case 0x03b2: return _color_make(0x6c, 0x40, 0x00);
      case 0x03d1: return _color_make(0x60, 0x38, 0x00);
      case 0x0340: return _color_make(0x60, 0x38, 0x00);
      case 0x0390: return _color_make(0x74, 0x44, 0x04);
      case 0x0044: return _color_make(0x60, 0x38, 0x00);
      case 0x0041: return _color_make(0x60, 0x38, 0x00);
      case 0x0341: return _color_make(0x60, 0x38, 0x00);
      case 0x03d0: return _color_make(0x60, 0x38, 0x00);
      case 0x0342: return _color_make(0x60, 0x38, 0x00);
      case 0x006d: return _color_make(0x48, 0x48, 0x48);
      case 0x0371: return _color_make(0x6c, 0x40, 0x00);
      case 0x0042: return _color_make(0x6c, 0x40, 0x00);
      case 0x004a: return _color_make(0x60, 0x38, 0x00);
      case 0x0391: return _color_make(0x60, 0x38, 0x00);
      case 0x0360: return _color_make(0x60, 0x38, 0x00);
      case 0x0321: return _color_make(0x60, 0x38, 0x00);
      case 0x03a0: return _color_make(0x6c, 0x40, 0x00);
      case 0x0036: return _color_make(0x6c, 0x40, 0x00);
      case 0x0310: return _color_make(0x74, 0x44, 0x04);
      case 0x03c1: return _color_make(0x6c, 0x40, 0x00);
      case 0x0330: return _color_make(0x74, 0x44, 0x04);
      case 0x0032: return _color_make(0x6c, 0x40, 0x00);
      case 0x0392: return _color_make(0x74, 0x44, 0x04);
      case 0x0300: return _color_make(0x60, 0x38, 0x00);
      case 0x0311: return _color_make(0x74, 0x44, 0x04);
      case 0x0350: return _color_make(0x60, 0x38, 0x00);
      case 0x0301: return _color_make(0x60, 0x38, 0x00);
      case 0x0035: return _color_make(0x6c, 0x40, 0x00);
      case 0x0320: return _color_make(0x6c, 0x40, 0x00);
      case 0x0550: return _color_make(0x28, 0x54, 0x0c);
      case 0x0581: return _color_make(0x74, 0x44, 0x04);
      case 0x0034: return _color_make(0x50, 0x30, 0x00);
      case 0x0551: return _color_make(0x2c, 0x5c, 0x10);
      case 0x003b: return _color_make(0x6c, 0x40, 0x00);
      case 0x03b0: return _color_make(0x74, 0x44, 0x04);
      case 0x0046: return _color_make(0x60, 0x38, 0x00);
      case 0x0480: return _color_make(0x68, 0x68, 0x68);
      case 0x0451: return _color_make(0x3c, 0x3c, 0x3c);
      case 0x0045: return _color_make(0x60, 0x38, 0x00);
      case 0x0047: return _color_make(0x60, 0x38, 0x00);
      case 0x03c0: return _color_make(0x6c, 0x40, 0x00);
      case 0x03b1: return _color_make(0x60, 0x38, 0x00);
      case 0x0048: return _color_make(0x60, 0x38, 0x00);
      case 0x0040: return _color_make(0x60, 0x38, 0x00);
      case 0x0580: return _color_make(0x6c, 0x40, 0x00);
      case 0x0781: return _color_make(0x00, 0x2c, 0x00);
      case 0x004b: return _color_make(0x60, 0x38, 0x00);
      case 0x0322: return _color_make(0x60, 0x38, 0x00);
      case 0x03a1: return _color_make(0x6c, 0x40, 0x00);
      case 0x0065: return _color_make(0x48, 0x48, 0x48);
      case 0x0450: return _color_make(0x3c, 0x3c, 0x3c);
      case 0x0170: return _color_make(0x04, 0x38, 0x74);
      case 0x01b2: return _color_make(0x04, 0x38, 0x74);
      case 0x01b0: return _color_make(0x04, 0x38, 0x74);
      case 0x0130: return _color_make(0x04, 0x38, 0x74);
      case 0x0191: return _color_make(0x04, 0x34, 0x70);
      case 0x0023: return _color_make(0x04, 0x34, 0x70);
      case 0x0020: return _color_make(0x04, 0x34, 0x70);
      case 0x0140: return _color_make(0x04, 0x34, 0x70);
      case 0x0820: return _color_make(0x50, 0x50, 0x50);
      case 0x08a0: return _color_make(0x88, 0x88, 0x88);
      case 0x0880: return _color_make(0x88, 0x88, 0x88);
      case 0x0890: return _color_make(0x88, 0x88, 0x88);
      case 0x0830: return _color_make(0x88, 0x88, 0x88);
      case 0x0171: return _color_make(0x04, 0x38, 0x74);
      case 0x01b1: return _color_make(0x04, 0x38, 0x74);
      case 0x01d1: return _color_make(0x04, 0x38, 0x74);
      case 0x0022: return _color_make(0x04, 0x34, 0x70);
      case 0x0142: return _color_make(0x04, 0x34, 0x70);
      case 0x0860: return _color_make(0x88, 0x88, 0x88);
      case 0x00b0: return _color_make(0x94, 0x94, 0x94);
      case 0x08d0: return _color_make(0x94, 0x94, 0x94);
      case 0x08c0: return _color_make(0x94, 0x94, 0x94);
      case 0x01d0: return _color_make(0x04, 0x38, 0x74);
      case 0x01c0: return _color_make(0x04, 0x34, 0x70);
      case 0x0131: return _color_make(0x04, 0x38, 0x74);
      case 0x0192: return _color_make(0x04, 0x38, 0x74);
      case 0x0021: return _color_make(0x04, 0x34, 0x70);
      case 0x01c1: return _color_make(0x04, 0x34, 0x70);
      case 0x0141: return _color_make(0x04, 0x34, 0x70);
      case 0x0161: return _color_make(0x04, 0x34, 0x70);
      case 0x01a0: return _color_make(0x04, 0x34, 0x70);
      case 0x0101: return _color_make(0x04, 0x34, 0x70);
      case 0x0122: return _color_make(0x04, 0x34, 0x70);
      case 0x0121: return _color_make(0x04, 0x34, 0x70);
      case 0x0120: return _color_make(0x04, 0x34, 0x70);
      case 0x0160: return _color_make(0x04, 0x34, 0x70);
      case 0x0100: return _color_make(0x04, 0x34, 0x70);
      case 0x0110: return _color_make(0x04, 0x38, 0x74);
      case 0x01a1: return _color_make(0x04, 0x34, 0x70);
      case 0x0111: return _color_make(0x04, 0x34, 0x70);
      case 0x0190: return _color_make(0x04, 0x38, 0x74);
      case 0x08b0: return _color_make(0x94, 0x94, 0x94);
      case 0x0850: return _color_make(0x88, 0x88, 0x88);
      case 0x0891: return _color_make(0x88, 0x88, 0x88);
      case 0x0841: return _color_make(0x88, 0x88, 0x88);
      case 0x0840: return _color_make(0x88, 0x88, 0x88);
      case 0x0481: return _color_make(0x50, 0x50, 0x50);
      case 0x0038: return _color_make(0x6c, 0x40, 0x00);
      case 0x0900: return _color_make(0x68, 0x68, 0x68);
      case 0x0950: return _color_make(0x68, 0x68, 0x68);
      case 0x0920: return _color_make(0x68, 0x68, 0x68);
      case 0x0940: return _color_make(0x68, 0x68, 0x68);
      case 0x0941: return _color_make(0x5c, 0x5c, 0x5c);
      case 0x0980: return _color_make(0x68, 0x68, 0x68);
      case 0x0990: return _color_make(0x68, 0x68, 0x68);
      case 0x0991: return _color_make(0x5c, 0x5c, 0x5c);
      case 0x09b0: return _color_make(0x3c, 0x3c, 0x3c);
      default:
                   ERR("Unhandled tile [0x%04x]", tile);
                   break;
     }

   return _color_make(0xff, 0x00, 0xff);
}

bool
pud_section_is_optional(Pud_Section sec)
{
   return ((sec == PUD_SECTION_ERAX) ||
           (sec == PUD_SECTION_ALOW));
}

uint32_t
pud_go_to_section(Pud         *pud,
                  Pud_Section  sec)
{
   FILE *f;
   uint8_t b;
   uint32_t l;
   char buf[4];
   const char *sec_str;

   PUD_SANITY_CHECK(pud, 0);
   if (sec > 19) DIE_RETURN(0, "Invalid section ID [%i]", sec);

   f = pud->file;
   sec_str = _sections[sec];

   /* If the section to search for is before the current section,
    * rewind the file to catch it. If it is after, do nothing */
   if (sec <= pud->current_section)
     rewind(f);

   l = file_read_long(f);
   if (ferror(f)) DIE_RETURN(false, "Error while reading file");
   memcpy(buf, &l, sizeof(uint32_t));

   while (!feof(f))
     {
        if (!strncmp(buf, sec_str, 4))
          {
             /* Update current section */
             pud->current_section = sec;
             fread(&l, sizeof(uint32_t), 1, f);
             PUD_CHECK_FERROR(f, 0);
             return l;
          }

        memmove(buf, &(buf[1]), 3 * sizeof(char));
        b = file_read_byte(f);
        if (ferror(f)) DIE_RETURN(false, "Error while reading file");
        buf[3] = b;
     }
   pud->current_section = PUD_SECTION_UNIT; // Go to last section

   return 0;
}



Pud *
pud_new(const char *file)
{
   Pud *pud;

   pud = calloc(1, sizeof(Pud));
   if (!pud) DIE_GOTO(err, "Failed to alloc Pud: %s", strerror(errno));

   pud->file = fopen(file, "r");
   if (!pud->file) DIE_GOTO(err_free, "Failed to open file [%s]", file);

   return pud;

err_free:
   free(pud);
err:
   return NULL;
}

void
pud_free(Pud *pud)
{
   fclose(pud->file);
   free(pud->units);
   free(pud->map_tiles);
   free(pud);
}

void
pud_verbose_set(Pud *pud,
                int  on)
{
   if (pud)
     pud->verbose = !!on;
}

void
pud_print(Pud  *pud,
          FILE *stream)
{
   int i;

   if (!stream) stream = stdout;

   fprintf(stream, "Tag ID..........: %x\n", pud->tag);
   fprintf(stream, "Version.........: %x\n", pud->version);
   fprintf(stream, "Description.....: %s\n", pud->description);
   fprintf(stream, "Era.............: %i\n", pud->era);
   fprintf(stream, "Dimensions......: %x\n", pud->dims);

   //   fprintf(stream, "Units & Building allowed

   fprintf(stream, "Starting Gold...:\n");
   for (i = 0; i < 8; i++)
     fprintf(stream, "   player %i.....: %u\n", i + 1, pud->sgld.players[i]);
   for (i = 0; i < 7; i++)
     fprintf(stream, "   unusable %i...: %u\n", i + 1, pud->sgld.unusable[i]);
   fprintf(stream, "   neutral......: %u\n", pud->sgld.neutral);

   fprintf(stream, "Starting Lumber.:\n");
   for (i = 0; i < 8; i++)
     fprintf(stream, "   player %i.....: %u\n", i + 1, pud->slbr.players[i]);
   for (i = 0; i < 7; i++)
     fprintf(stream, "   unusable %i...: %u\n", i + 1, pud->slbr.unusable[i]);
   fprintf(stream, "   neutral......: %u\n", pud->slbr.neutral);

   fprintf(stream, "Starting Oil....:\n");
   for (i = 0; i < 8; i++)
     fprintf(stream, "   player %i.....: %u\n", i + 1, pud->soil.players[i]);
   for (i = 0; i < 7; i++)
     fprintf(stream, "   unusable %i...: %u\n", i + 1, pud->soil.unusable[i]);
   fprintf(stream, "   neutral......: %u\n", pud->soil.neutral);

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

int
pud_parse(Pud *pud)
{
   pud_parse_type(pud);
   pud_parse_ver(pud);
   pud_parse_desc(pud);
   pud_parse_era(pud);
   pud_parse_dim(pud);
   pud_parse_udta(pud);
   pud_parse_alow(pud);
   pud_parse_ugrd(pud);
   pud_parse_sgld(pud);
   pud_parse_slbr(pud);
   pud_parse_soil(pud);
   pud_parse_mtxm(pud);
   pud_parse_oilm(pud);
   pud_parse_regm(pud);
   pud_parse_unit(pud);

   // pud_print(pud, stdout);


   return 0;
}

/*============================================================================*
 *                              Parsing Routines                              *
 *============================================================================*/

bool
pud_parse_type(Pud *pud)
{
   PUD_SANITY_CHECK(pud, 0);

   char buf[16];
   FILE *f = pud->file;
   uint32_t l;
   uint32_t chk;

   chk = pud_go_to_section(pud, PUD_SECTION_TYPE);
   if (!chk) DIE_RETURN(false, "Failed to reach section TYPE");
   PUD_VERBOSE(pud, "At section TYPE (size = %u)", chk);

   /* Read 10bytes + 2 unused */
   fread(buf, sizeof(uint8_t), 12, f);
   PUD_CHECK_FERROR(f, false);
   if (strncmp(buf, "WAR2 MAP\0\0", 10))
     DIE_RETURN(false, "TYPE section has a wrong header");

   /* Read ID TAG */
   l = file_read_long(f);
   PUD_CHECK_FERROR(f, false);

   pud->tag = l;

   return true;
}

bool
pud_parse_ver(Pud *pud)
{
   PUD_SANITY_CHECK(pud, false);

   FILE *f = pud->file;
   uint16_t w;
   uint32_t chk;

   chk = pud_go_to_section(pud, PUD_SECTION_VER);
   if (!chk) DIE_RETURN(false, "Failed to reach section VER");
   PUD_VERBOSE(pud, "At section VER (size = %u)", chk);

   fread(&w, sizeof(uint16_t), 1, f);
   PUD_CHECK_FERROR(f, false);

   pud->version = w;
   return true;
}

bool
pud_parse_desc(Pud *pud)
{
   PUD_SANITY_CHECK(pud, false);

   uint32_t chk;
   FILE *f = pud->file;
   char buf[32];

   chk = pud_go_to_section(pud, PUD_SECTION_DESC);
   if (!chk) DIE_RETURN(false, "Failed to reach section DESC");
   PUD_VERBOSE(pud, "At section DESC (size = %u)", chk);

   fread(buf, sizeof(char), 32, f);
   PUD_CHECK_FERROR(f, false);

   memcpy(pud->description, buf, 32 * sizeof(char));
   return true;
}

/*
 * TODO pud_parse_ownr()
 */

bool
pud_parse_era(Pud *pud)
{
   PUD_SANITY_CHECK(pud, false);

   uint32_t chk;
   FILE *f = pud->file;
   uint16_t w;

   chk = pud_go_to_section(pud, PUD_SECTION_ERAX);
   if (!chk) // Optional section, use ERA by default
     {
        PUD_VERBOSE(pud, "Failed to find ERAX. Trying with ERA...");
        chk = pud_go_to_section(pud, PUD_SECTION_ERA);
        if (!chk) DIE_RETURN(false, "Failed to reach section ERA");
        PUD_VERBOSE(pud, "At section ERA (size = %u)", chk);
     }

   fread(&w, sizeof(uint16_t), 1, f);
   PUD_CHECK_FERROR(f, false);

   switch (w)
     {
      case 0x00:
      case 0x04 ... 0xff:
         pud->era = PUD_ERA_FOREST;
         break;

      case 0x01:
         pud->era = PUD_ERA_WINTER;
         break;

      case 0x02:
         pud->era = PUD_ERA_WASTELAND;
         break;

      case 0x03:
         pud->era = PUD_ERA_SWAMP;
         break;

      default:
         DIE_RETURN(false, "Failed to parse Era [%x]", w);
         break;
     }

   return true;
}

bool
pud_parse_dim(Pud *pud)
{
   PUD_SANITY_CHECK(pud, false);

   uint32_t chk;
   FILE *f = pud->file;
   uint16_t x, y;

   chk = pud_go_to_section(pud, PUD_SECTION_DIM);
   if (!chk) DIE_RETURN(false, "Failed to reach section DIM");
   PUD_VERBOSE(pud, "At section DIM (size = %u)", chk);

   fread(&x, sizeof(uint16_t), 1, f);
   fread(&y, sizeof(uint16_t), 1, f);
   PUD_CHECK_FERROR(f, false);

   if ((x == 32) && (y == 32))
     pud->dims = PUD_DIMENSIONS_32_32;
   else if ((x == 64) && (y == 64))
     pud->dims = PUD_DIMENSIONS_64_64;
   else if ((x == 96) && (y == 96))
     pud->dims = PUD_DIMENSIONS_96_96;
   else if ((x == 128) && (y == 128))
     pud->dims = PUD_DIMENSIONS_128_128;
   else
     return false;

   pud_dimensions_to_size(pud->dims, &pud->map_w, &pud->map_h);

   return true;
}

bool
pud_parse_udta(Pud *pud)
{
   PUD_SANITY_CHECK(pud, false);

   uint32_t chk;
   FILE *f = pud->file;
   uint16_t wb[512];
   uint32_t lb[128];
   uint8_t bb[128];
   int i;

   chk = pud_go_to_section(pud, PUD_SECTION_UDTA);
   if (!chk) DIE_RETURN(false, "Failed to reach section UDTA");
   PUD_VERBOSE(pud, "At section UDTA (size = %u)", chk);

   /* Use default data */
   fread(wb, sizeof(uint16_t), 1, f);
   PUD_CHECK_FERROR(f, false);
   pud->default_udta = !!wb[0];

   /* Overlap frames */
   fread(wb, sizeof(uint16_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].overlap_frames = wb[i];

   /* Obsolete data */
   fread(wb, sizeof(uint16_t), 508, f);
   PUD_CHECK_FERROR(f, false);

   /* Sight (why the hell is it on 32 bits!?) */
   fread(lb, sizeof(uint32_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].sight = lb[i];

   /* Hit points */
   fread(wb, sizeof(uint16_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].hp = wb[i];

   /* Magic */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].has_magic = !!bb[i];

   /* Build time */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].build_time = bb[i];

   /* Gold cost */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].gold_cost = bb[i];

   /* Lumber cost */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].lumber_cost = bb[i];

   /* Oil cost */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].oil_cost = bb[i];

   /* Unit size */
   fread(lb, sizeof(uint32_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     {
        pud->unit_data[i].size_w = (lb[i] >> 16) & 0x0000ffff;
        pud->unit_data[i].size_h = lb[i] & 0x0000ffff;
     }

   /* Unit box */
   fread(lb, sizeof(uint32_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     {
        pud->unit_data[i].box_w = (lb[i] >> 16) & 0x0000ffff;
        pud->unit_data[i].box_h = lb[i] & 0x0000ffff;
     }

   /* Attack range */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].range = bb[i];

   /* React range (computer) */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].computer_react_range = bb[i];

   /* React range (human) */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].human_react_range = bb[i];

   /* Armor */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].armor = bb[i];

   /* Selectable via rectangle */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].rect_sel = !!bb[i];

   /* Priority */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].priority = bb[i];

   /* Basic damage */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].basic_damage = bb[i];

   /* Piercing damage */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].piercing_damage = bb[i];

   /* Weapons upgradable */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].weapons_upgradable = !!bb[i];

   /* Armor upgradable */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].armor_upgradable = !!bb[i];

   /* Missile weapon */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].missile_weapon = bb[i];

   /* Unit type */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].type = bb[i];

   /* Decay rate */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].decay_rate = bb[i];

   /* Annoy computer factor */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].annoy = bb[i];

   /* 2nd mouse button action */
   fread(bb, sizeof(uint8_t), 58, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 58; i++)
     pud->unit_data[i].mouse_right_btn = bb[i];

   /* Point value for killing unit */
   fread(wb, sizeof(uint16_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].point_value = wb[i];

   /* Can target */
   fread(bb, sizeof(uint8_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].can_target = bb[i];

   /* Flags */
   fread(lb, sizeof(uint32_t), 110, f);
   PUD_CHECK_FERROR(f, false);
   for (i = 0; i < 110; i++)
     pud->unit_data[i].flags = lb[i];

   /* Obsolete */
   fread(wb, sizeof(uint16_t), 127, f);
   PUD_CHECK_FERROR(f, false);

   return true;
}

bool
pud_parse_alow(Pud *pud)
{
   PUD_SANITY_CHECK(pud, false);

   uint32_t chk;
   FILE *f = pud->file;
   uint32_t buf[16];
   struct _alow *ptrs[] = {
      &(pud->unit_alow),
      &(pud->spell_start),
      &(pud->spell_alow),
      &(pud->spell_acq),
      &(pud->up_alow),
      &(pud->up_acq)
   };
   const int ptrs_count = sizeof(ptrs) / sizeof(void *);
   int i;

   pud->default_allow = 0; // Reset before checking
   chk = pud_go_to_section(pud, PUD_SECTION_ALOW);
   if (!chk)
     {
        PUD_VERBOSE(pud, "Section ALOW (optional) not present. Skipping...");
        pud->default_allow = 1;
        return true;
     }
   PUD_VERBOSE(pud, "At section ALOW (size = %u)", chk);

   for (i = 0; i < ptrs_count; i++)
     {
        fread(buf, sizeof(uint32_t), 16, f);
        PUD_CHECK_FERROR(f, false);

        memcpy(&(ptrs[i]->players[0]),  &(buf[0]),  sizeof(uint32_t) * 8);
        memcpy(&(ptrs[i]->unusable[0]), &(buf[8]),  sizeof(uint32_t) * 7);
        memcpy(&(ptrs[i]->neutral),     &(buf[15]), sizeof(uint32_t) * 1);
     }

   return true;
}

bool
pud_parse_ugrd(Pud *pud)
{
   PUD_SANITY_CHECK(pud, false);

   uint32_t chk;
   FILE *f = pud->file;
   int i;
   uint8_t buf[208];

   chk = pud_go_to_section(pud, PUD_SECTION_UGRD);
   if (!chk) DIE_RETURN(false, "Failed to reach section UGRD");
   PUD_VERBOSE(pud, "At section UGRD (size = %u)", chk);

   for (i = 0; i < 52; i++)
     {
     }

   return true;
}

bool
pud_parse_sgld(Pud *pud)
{
   PUD_SANITY_CHECK(pud, false);

   uint32_t chk;
   FILE *f = pud->file;
   uint16_t buf[16];

   chk = pud_go_to_section(pud, PUD_SECTION_SGLD);
   if (!chk) DIE_RETURN(false, "Failed to reach section SGLD");
   PUD_VERBOSE(pud, "At section SGLD (size = %u)", chk);

   fread(buf, sizeof(uint16_t), 16, f);
   PUD_CHECK_FERROR(f, false);

   memcpy(&(pud->sgld.players[0]),  &(buf[0]),  sizeof(uint16_t) * 8);
   memcpy(&(pud->sgld.unusable[0]), &(buf[8]),  sizeof(uint16_t) * 7);
   memcpy(&(pud->sgld.neutral),     &(buf[15]), sizeof(uint16_t) * 1);

   return true;
}

bool
pud_parse_slbr(Pud *pud)
{
   PUD_SANITY_CHECK(pud, false);

   uint32_t chk;
   FILE *f = pud->file;
   uint16_t buf[16];

   chk = pud_go_to_section(pud, PUD_SECTION_SLBR);
   if (!chk) DIE_RETURN(false, "Failed to reach section SLBR");
   PUD_VERBOSE(pud, "At section SLBR (size = %u)", chk);

   fread(buf, sizeof(uint16_t), 16, f);
   PUD_CHECK_FERROR(f, false);

   memcpy(&(pud->slbr.players[0]),  &(buf[0]),  sizeof(uint16_t) * 8);
   memcpy(&(pud->slbr.unusable[0]), &(buf[8]),  sizeof(uint16_t) * 7);
   memcpy(&(pud->slbr.neutral),     &(buf[15]), sizeof(uint16_t) * 1);

   return true;
}

bool
pud_parse_soil(Pud *pud)
{
   PUD_SANITY_CHECK(pud, false);

   uint32_t chk;
   FILE *f = pud->file;
   uint16_t buf[16];

   chk = pud_go_to_section(pud, PUD_SECTION_SOIL);
   if (!chk) DIE_RETURN(false, "Failed to reach section SOIL");
   PUD_VERBOSE(pud, "At section SOIL (size = %u)", chk);

   fread(buf, sizeof(uint16_t), 16, f);
   PUD_CHECK_FERROR(f, false);

   memcpy(&(pud->soil.players[0]),  &(buf[0]),  sizeof(uint16_t) * 8);
   memcpy(&(pud->soil.unusable[0]), &(buf[8]),  sizeof(uint16_t) * 7);
   memcpy(&(pud->soil.neutral),     &(buf[15]), sizeof(uint16_t) * 1);

   return true;
}

bool
pud_parse_mtxm(Pud *pud)
{
   PUD_SANITY_CHECK(pud, false);

   uint32_t chk;
   FILE *f = pud->file;
   uint16_t w;
   int i;
   int size;

   chk = pud_go_to_section(pud, PUD_SECTION_MTXM);
   if (!chk) DIE_RETURN(false, "Failed to reach section MTXM");
   PUD_VERBOSE(pud, "At section MTXM (size = %u)", chk);

   /* Check for integrity */
   size = pud->map_w * pud->map_h;
   if ((size * sizeof(uint16_t)) != chk)
     DIE_RETURN(false, "Mismatch between dims and tiles number");

   pud->map_tiles = calloc(size, sizeof(uint16_t));
   if (!pud->map_tiles) DIE_RETURN(false, "Failed to allocate memory");
   pud->map_tiles_count = size;

   for (i = 0; i < size; i++)
     {
        fread(&w, sizeof(uint16_t), 1, f);
        PUD_CHECK_FERROR(f, false);
        pud->map_tiles[i] = w;
     }

   return true;
}

bool
pud_parse_oilm(Pud *pud)
{
   PUD_SANITY_CHECK(pud, false);

   uint32_t chk;

   chk = pud_go_to_section(pud, PUD_SECTION_OILM);
   if (!chk) PUD_VERBOSE(pud, "Section OILM (obsolete) not present. Skipping...");
   else PUD_VERBOSE(pud, "Section OILM (obsolete) present. Skipping...");

   return true;
}

bool
pud_parse_regm(Pud *pud)
{
   PUD_SANITY_CHECK(pud, false);

   uint32_t chk;
   FILE *f = pud->file;
   uint16_t w;

   chk = pud_go_to_section(pud, PUD_SECTION_REGM);
   if (!chk) DIE_RETURN(false, "Failed to reach section REGM");
   PUD_VERBOSE(pud, "At section REGM (size = %u)", chk);

   //   for (int i = 0; i < 128 * 128; i ++)
   //     {
   //   w = file_read_word(f);
   //   printf("-> %x\n", w);
   //     }
   //
   //   return true;

   return false;
}

bool
pud_parse_unit(Pud *pud)
{
   PUD_SANITY_CHECK(pud, false);

   uint32_t chk;
   FILE *f = pud->file;
   int i, units;
   struct _unit u;

   chk = pud_go_to_section(pud, PUD_SECTION_UNIT);
   if (!chk) DIE_RETURN(false, "Failed to reach section UNIT");
   PUD_VERBOSE(pud, "At section UNIT (size = %u)", chk);
   units = chk / 8;

   pud->units = calloc(units, sizeof(struct _unit));
   if (!pud->units) DIE_RETURN(false, "Failed to allocate memory");
   pud->units_count = units;

   fread(pud->units, sizeof(struct _unit), units, f);
   PUD_CHECK_FERROR(f, false);

   return true;
}



/*============================================================================*
 *                              Output to images                              *
 *============================================================================*/

static unsigned char *
_minimap_bitmap_generate(Pud *pud,
                         int *size_ret)
{
   PUD_SANITY_CHECK(pud, NULL);

   unsigned char *map;
   struct _unit *u;
   Color c;
   int i;
   int idx;
   int size;
   uint16_t w;

   size = pud->map_tiles_count * 3;
   map = calloc(size, sizeof(unsigned char));
   if (!map) DIE_RETURN(NULL, "Failed to allocate memory");

   for (i = 0, idx = 0; i < pud->map_tiles_count; i++, idx += 3)
     {
        c = _pud_tile_to_color(pud, pud->map_tiles[i]);

        map[idx + 0] = c.r;
        map[idx + 1] = c.g;
        map[idx + 2] = c.b;
     }

   for (i = 0; i < pud->units_count; i++)
     {
        u = &(pud->units[i]);
        c = _color_for_player(u->owner);

        idx = ((u->y * pud->map_w) + u->x) * 3;

        map[idx + 0] = c.r;
        map[idx + 1] = c.g;
        map[idx + 2] = c.b;
     }

   if (size_ret) *size_ret = size;

   return map;
}

bool
pud_minimap_to_ppm(Pud        *pud,
                   const char *file)
{
   PUD_SANITY_CHECK(pud, false);

   FILE *f;
   int i, size;
   unsigned char *map;

   map = _minimap_bitmap_generate(pud, &size);
   if (!map) DIE_RETURN(false, "Failed to generate bitmap");

   f = fopen(file, "w");
   if (!f) DIE_RETURN(false, "Failed to open [%s]", file);

   /* Write PPM header */
   fprintf(f,
           "P3\n"
           "%i %i\n"
           "255\n",
           pud->map_w, pud->map_h);

   for (i = 0; i < size; i += 3)
     {
        fprintf(f, "%i %i %i\n",
                map[i + 0],
                map[i + 1],
                map[i + 2]);
     }
   fclose(f);
   free(map);

   return true;
}

bool
pud_minimap_to_jpeg(Pud        *pud,
                    const char *file)
{
   PUD_SANITY_CHECK(pud, false);

   unsigned char *map;
   bool chk;

   map = _minimap_bitmap_generate(pud, NULL);
   if (!map) DIE_RETURN(false, "Failed to generate bitmap");

   chk = jpeg_write(file, pud->map_w, pud->map_h, map);
   free(map);

   return chk;
}

uint16_t
pud_tile_at(Pud *pud,
            int  x,
            int  y)
{
   PUD_SANITY_CHECK(pud, 0);

   if (((unsigned int)(x * y)) >= (unsigned int)pud->map_tiles_count)
     DIE_RETURN(0, "Invalid coordinates %i,%i", x, y);

   return pud->map_tiles[y * pud->map_w + x];
}

