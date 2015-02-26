#ifndef _EDITOR_H_
#define _EDITOR_H_

typedef struct _Editor Editor;

#define EDITOR_ITEM_HUMAN       (1 << 0)
#define EDITOR_ITEM_ORC         (1 << 1)

struct _Editor
{
   Pud          *pud;
   Evas_Object  *win;
   Evas_Object  *menu;
   Evas_Object  *hover;
   Evas_Object  *inwin;
   Evas_Object  *scroller;
   Evas_Object  *table;

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
   Pud_Era era;
   Eina_Bool has_extension;
};

Eina_Bool editor_init(void);
void editor_shutdown(void);
void editor_free(Editor *ed);
void editor_close(Editor *ed);
Editor *editor_new(void);
void editor_mainconfig_show(Editor *ed);
void editor_mainconfig_hide(Editor *ed);

#endif /* ! _EDITOR_H_ */

