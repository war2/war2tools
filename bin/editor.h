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

   Elm_Object_Item *hmn_sel[4];
   Elm_Object_Item *orc_sel[4];

   struct _menu_item {
      Elm_Object_Item *item;
      Evas_Object     *radio;
      unsigned char    active_for : 2;
   } tools_items[110], *tools_item_active;
};

Eina_Bool editor_init(void);
void editor_shutdown(void);
void editor_free(Editor *ed);
Editor *editor_new(Pud_Era era, Pud_Dimensions dims);

#endif /* ! _EDITOR_H_ */

