#ifndef _EDITOR_H_
#define _EDITOR_H_


#define EDITOR_ITEM_HUMAN       (1 << 0)
#define EDITOR_ITEM_ORC         (1 << 1)

struct _Editor
{
   Pud          *pud;
   Evas_Object  *win;
   Evas_Object  *menu;
   Evas_Object  *inwin;
   Evas_Object  *mainbox;
   Evas_Object  *scroller;
   Grid_Cell   **cells;

   Evas_Object   *bitmap;
   unsigned char *pixels;

   Eet_File     *units;
   Eet_File     *buildings;
   Eina_Hash    *sprites;

   char era_str[16];

   Eet_File     *textures_src;
   Eina_Hash    *textures;
   Texture_Dictionary tex_dict;

   Elm_Object_Item *main_sel[4];
   Elm_Object_Item *hmn_sel[4];
   Elm_Object_Item *orc_sel[4];

   struct _mainconfig {
      Evas_Object *container;
      Evas_Object *img;
      Evas_Object *menu_size;
      Evas_Object *menu_era;
   } mainconfig;

   struct _menu_item {
      Elm_Object_Item *item;
      Evas_Object     *radio;
      unsigned char    active_for : 2;
   } tools_items[110], *tools_item_active;

   Pud_Dimensions size;
   int map_w;
   int map_h;
   int bitmap_w;
   int bitmap_h;
   Pud_Era era;
   Eina_Bool has_extension;
};

Eina_Bool editor_init(void);
void editor_shutdown(void);
void editor_free(Editor *ed);
Editor *editor_new(void);
void editor_mainconfig_show(Editor *ed);
void editor_mainconfig_hide(Editor *ed);
void editor_error(Editor *ed, const char *msg);

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

