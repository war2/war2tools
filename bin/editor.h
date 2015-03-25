#ifndef _EDITOR_H_
#define _EDITOR_H_

typedef enum
{
   EDITOR_SPREAD_NORMAL,
   EDITOR_SPREAD_CIRCLE,
   EDITOR_SPREAD_RANDOM
} Editor_Spread;

typedef enum
{
   EDITOR_RADIUS_SMALL,
   EDITOR_RADIUS_MEDIUM,
   EDITOR_RADIUS_BIG
} Editor_Radius;

typedef enum
{
   EDITOR_ACTION_SELECTION,
   EDITOR_ACTION_WATER,
   EDITOR_ACTION_NON_CONSTRUCTIBLE,
   EDITOR_ACTION_CONSTRUCTIBLE,
   EDITOR_ACTION_TREES,
   EDITOR_ACTION_ROCKS,
   EDITOR_ACTION_HUMAN_WALLS,
   EDITOR_ACTION_ORCS_WALLS
} Editor_Action;

typedef enum
{
   EDITOR_TINT_LIGHT,
   EDITOR_TINT_DARK
} Editor_Tint;


struct _Editor
{
   Pud          *pud;
   Evas_Object  *win;
   Evas_Object  *menu;
   Evas_Object  *inwin;
   Evas_Object  *mainbox;
   Evas_Object  *scroller;
   Evas_Object  *fs; /* File selector */
   Cell   **cells;

   Evas_Object   *bitmap;
   unsigned char *pixels;

   Eet_File     *units;
   Eet_File     *buildings;
   Eina_Hash    *sprites;

   char era_str[16];

   Eet_File     *textures_src;
   Eina_Hash    *textures;
   Texture_Dictionary tex_dict;

   /* Toolbar */
   Evas_Object *toolbar;
   Evas_Object *segments[4];
   Editor_Action action;
   Editor_Spread spread;
   Editor_Radius radius;
   Editor_Tint   tint;
   Pud_Unit      sel_unit;
   Pud_Player    sel_player;

   Eina_Stringshare *save_file;

   /* Cursor */
   struct {
      Evas_Object *obj;
      int          x;
      int          y;
      int          w;
      int          h;
      Eina_Bool    enabled;
   } cursor;

   struct _mainconfig {
      Evas_Object *container;
      Evas_Object *img;
      Evas_Object *menu_size;
      Evas_Object *menu_era;
   } mainconfig;

   Evas_Point start_locations[8];
   Evas_Object *radio_units_reset;

   /* Orc, Human */
   uint8_t sides[8];

   Evas_Point bitmap_origin;

   Pud_Dimensions size;
   int map_w;
   int map_h;
   int bitmap_w;
   int bitmap_h;

   int bmp_step_w;
   int bmp_step_h;

   Pud_Era era;
   Eina_Bool has_extension;

};

Eina_Bool editor_init(void);
void editor_shutdown(void);

void editor_free(Editor *ed);
Editor *editor_new(void);
void editor_error(Editor *ed, const char *msg);
void editor_finalize(Editor *ed);

#define EDITOR_ERROR_RET(ed_, msg_, ...) \
   do { \
      CRI(msg_); \
      editor_error(ed_, msg_); \
      return __VA_ARGS__; \
   } while (0)

#define EDITOR_ERROR_GOTO(ed_, msg_, label_) \
   do { \
      CRI(msg_); \
      editor_error(ed_, msg_); \
      goto label_; \
   } while (0)

#endif /* ! _EDITOR_H_ */

