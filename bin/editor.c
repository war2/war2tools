#include "war2edit.h"

static Eina_List *_windows;

/*============================================================================*
 *                                  Callbacks                                 *
 *============================================================================*/

static void
_win_del_cb(void        *data,
            Evas_Object *obj   EINA_UNUSED,
            void        *event EINA_UNUSED)
{
   Editor *ed = data;

   evas_object_del(ed->win);
   _windows = eina_list_remove(_windows, ed);
}

static void
_win_focused_cb(void        *data  EINA_UNUSED,
                Evas_Object *obj   EINA_UNUSED,
                void        *event EINA_UNUSED)
{
//   Editor *ed = data;
}

static void
_win_new_cb(void        *data  EINA_UNUSED,
            Evas_Object *obj   EINA_UNUSED,
            void        *event EINA_UNUSED)
{
}

static void
_win_open_cb(void        *data  EINA_UNUSED,
             Evas_Object *obj   EINA_UNUSED,
             void        *event EINA_UNUSED)
{
}

static void
_win_save_cb(void        *data  EINA_UNUSED,
             Evas_Object *obj   EINA_UNUSED,
             void        *event EINA_UNUSED)
{
}

static void
_win_save_as_cb(void        *data  EINA_UNUSED,
                Evas_Object *obj   EINA_UNUSED,
                void        *event EINA_UNUSED)
{
}

static Editor *
_editor_for_menu(Evas_Object *menu)
{
   Eina_List *l;
   Editor *e;

   EINA_LIST_FOREACH(_windows, l, e)
      if (e->menu == menu)
        return e;

   return NULL;
}

static void
_radio_cb(void        *data,
          Evas_Object *obj,
          void        *event EINA_UNUSED)
{
   struct _menu_item *mi = data;
   Editor *ed;

   ed = _editor_for_menu(obj);
   EINA_SAFETY_ON_NULL_RETURN(ed);

   /* Already active: do nothing */
   if (ed->tools_item_active != mi)
     {
        /* Disable the already active item */
        if (ed->tools_item_active)
          elm_radio_value_set(ed->tools_item_active->radio, 0);
        /* Enable the new active item */
        elm_radio_value_set(mi->radio, 1);
        ed->tools_item_active = mi;
     }
}

static Elm_Object_Item *
_radio_add(Editor          *editor,
           unsigned int     idx,
           Elm_Object_Item *parent,
           const char      *label,
           unsigned int     flags)
{
   Evas_Object *o;
   Elm_Object_Item *eoi;
   struct _menu_item *mi;

   mi = &(editor->tools_items[idx]);

   o = elm_radio_add(editor->menu);
   elm_radio_state_value_set(o, 1);
   elm_radio_value_set(o, 0);
   elm_object_text_set(o, label);

   eoi = elm_menu_item_add(editor->menu, parent, NULL, NULL, _radio_cb, mi);
   elm_object_item_content_set(eoi, o);

   mi->radio = o;
   mi->item = eoi;

   return eoi;
}



/*============================================================================*
 *                                 Public API                                 *
 *============================================================================*/

Eina_Bool
editor_init(void)
{
   elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);
   return EINA_TRUE;
}

void
editor_shutdown(void)
{
   Editor *ed;

   EINA_LIST_FREE(_windows, ed)
      editor_free(ed);
   _windows = NULL;
}

void
editor_free(Editor *ed)
{
   if (!ed) return;

   pud_close(ed->pud);
   free(ed);
}

Editor *
editor_new(Pud_Era        era,
           Pud_Dimensions dims)
{
   Editor *ed;
   char filename[512], *ptr;
   char title[128], wins[32];
   Evas_Object *o;
   Elm_Object_Item *itm, *i;
   int k = 0;

   /* Get unique filename */
   snprintf(filename, sizeof(filename), "/tmp/war2dit.XXXXXX");
   ptr = mktemp(filename);
   EINA_SAFETY_ON_NULL_GOTO(ptr, err_ret);

   ed = calloc(1, sizeof(Editor));
   EINA_SAFETY_ON_NULL_GOTO(ed, err_ret);

   /* Create window's title */
   snprintf(wins, sizeof(wins), " - %i", eina_list_count(_windows));
   snprintf(title, sizeof(title), "Untitled%s",
            (eina_list_count(_windows) == 0) ? "" : wins);

   /* Create window and set callbacks */
   ed->win = elm_win_util_standard_add("win-editor", title);
   EINA_SAFETY_ON_NULL_GOTO(ed->win, err_free);
   elm_win_focus_highlight_enabled_set(ed->win, EINA_TRUE);
   evas_object_smart_callback_add(ed->win, "focus,in", _win_focused_cb, ed);
   evas_object_smart_callback_add(ed->win, "delete,request", _win_del_cb, ed);
   evas_object_resize(ed->win, 640, 480);

   /* Add a box to put widgets in it */
   o = elm_box_add(ed->win);
   EINA_SAFETY_ON_NULL_GOTO(o, err_win_del);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(ed->win, o);
   evas_object_show(o);

   /* Get the main menu */
   ed->menu = elm_win_main_menu_get(ed->win);
   EINA_SAFETY_ON_NULL_GOTO(ed->menu, err_win_del);
   itm = elm_menu_item_add(ed->menu, NULL, NULL,  "File", NULL, NULL);
   elm_menu_item_add(ed->menu, itm, NULL, "New...", _win_new_cb, NULL);
   elm_menu_item_add(ed->menu, itm, NULL, "Open...", _win_open_cb, NULL);
   elm_menu_item_add(ed->menu, itm, NULL, "Save", _win_save_cb, ed);
   elm_menu_item_add(ed->menu, itm, NULL, "Save As...", _win_save_as_cb, ed);
   elm_menu_item_separator_add(ed->menu, itm);
   elm_menu_item_add(ed->menu, itm, NULL, "Quit", _win_del_cb, ed);

   itm = elm_menu_item_add(ed->menu, NULL, NULL, "Tools", NULL, NULL);

#define RADIO_ADD(flags, label) _radio_add(ed, k++, i, label, flags)

   i = itm;
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Human Start Location");

   i = ed->hmn_sel[0] = elm_menu_item_add(ed->menu, itm, NULL, "Human Air", NULL, NULL);
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Gnomish Flying Machine");
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Gryphon Rider");

   i = ed->hmn_sel[1] = elm_menu_item_add(ed->menu, itm, NULL, "Human Land", NULL, NULL);
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Peasant");
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Footman");
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Elven Archer");
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Knight");
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Balista");
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Dwarven Demolition Squad");
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Mage");

   i = ed->hmn_sel[2] = elm_menu_item_add(ed->menu, itm, NULL, "Human Water", NULL, NULL);
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Human Tanker");
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Elven Destroyer");
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Battleship");
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Human Transport");
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Gnomish Submarine");

   i = ed->hmn_sel[3] = elm_menu_item_add(ed->menu, itm, NULL, "Human Buildings", NULL, NULL);
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Farm");
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Town Hall");
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Keep");
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Castle");
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Human Barracks");
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Human Shipyard");
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Elven Lumber Mill");
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Human Foundry");
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Human Refinery");
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Human Oil Platform");
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Human Blacksmith");
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Stables");
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Church");
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Gnomish Inventor");
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Gryphon Aviary");
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Human Scout Tower");
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Human Guard Tower");
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Human Cannon Tower");
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Mage Tower");

   elm_menu_item_separator_add(ed->menu, itm);

   i = itm;
   RADIO_ADD(EDITOR_ITEM_ORC, "Orc Start Location");

   i = ed->orc_sel[0] = elm_menu_item_add(ed->menu, itm, NULL, "Orc Air", NULL, NULL);
   RADIO_ADD(EDITOR_ITEM_ORC, "Goblin Zepplin");
   RADIO_ADD(EDITOR_ITEM_ORC, "Dragon");

   i = ed->orc_sel[1] = elm_menu_item_add(ed->menu, itm, NULL, "Orc Land", NULL, NULL);
   RADIO_ADD(EDITOR_ITEM_ORC, "Peon");
   RADIO_ADD(EDITOR_ITEM_ORC, "Grunt");
   RADIO_ADD(EDITOR_ITEM_ORC, "Troll Axethrower");
   RADIO_ADD(EDITOR_ITEM_ORC, "Ogre");
   RADIO_ADD(EDITOR_ITEM_ORC, "Catapult");
   RADIO_ADD(EDITOR_ITEM_ORC, "Goblin Sapper");
   RADIO_ADD(EDITOR_ITEM_ORC, "Death Knight");

   i = ed->orc_sel[2] = elm_menu_item_add(ed->menu, itm, NULL, "Orc Water", NULL, NULL);
   RADIO_ADD(EDITOR_ITEM_ORC, "Orc Tanker");
   RADIO_ADD(EDITOR_ITEM_ORC, "Troll Destroyer");
   RADIO_ADD(EDITOR_ITEM_ORC, "Juggernaught");
   RADIO_ADD(EDITOR_ITEM_ORC, "Orc Transport");
   RADIO_ADD(EDITOR_ITEM_ORC, "Giant Turtle");

   i = ed->orc_sel[3] = elm_menu_item_add(ed->menu, itm, NULL, "Orc Buildings", NULL, NULL);
   RADIO_ADD(EDITOR_ITEM_ORC, "Pig Farm");
   RADIO_ADD(EDITOR_ITEM_ORC, "Great Hall");
   RADIO_ADD(EDITOR_ITEM_ORC, "Stronghold");
   RADIO_ADD(EDITOR_ITEM_ORC, "Fortress");
   RADIO_ADD(EDITOR_ITEM_ORC, "Orc Barracks");
   RADIO_ADD(EDITOR_ITEM_ORC, "Orc Shipyard");
   RADIO_ADD(EDITOR_ITEM_ORC, "Troll Lumber Mill");
   RADIO_ADD(EDITOR_ITEM_ORC, "Orc Foundry");
   RADIO_ADD(EDITOR_ITEM_ORC, "Orc Refinery");
   RADIO_ADD(EDITOR_ITEM_ORC, "Orc Oil Platform");
   RADIO_ADD(EDITOR_ITEM_ORC, "Orc Blacksmith");
   RADIO_ADD(EDITOR_ITEM_ORC, "Ogre Mound");
   RADIO_ADD(EDITOR_ITEM_ORC, "Altar of Storms");
   RADIO_ADD(EDITOR_ITEM_ORC, "Goblin Alchemist");
   RADIO_ADD(EDITOR_ITEM_ORC, "Dragon Roost");
   RADIO_ADD(EDITOR_ITEM_ORC, "Orc Scout Tower");
   RADIO_ADD(EDITOR_ITEM_ORC, "Orc Guard Tower");
   RADIO_ADD(EDITOR_ITEM_ORC, "Orc Cannon Tower");
   RADIO_ADD(EDITOR_ITEM_ORC, "Temple of the Damned");

   elm_menu_item_separator_add(ed->menu, itm);

   i = itm;
   RADIO_ADD(EDITOR_ITEM_HUMAN | EDITOR_ITEM_ORC, "Gold Mine");
   RADIO_ADD(EDITOR_ITEM_HUMAN | EDITOR_ITEM_ORC, "Oil Patch");
   RADIO_ADD(EDITOR_ITEM_HUMAN | EDITOR_ITEM_ORC, "Critter");
   RADIO_ADD(EDITOR_ITEM_HUMAN | EDITOR_ITEM_ORC, "Circle of Power");
   RADIO_ADD(EDITOR_ITEM_HUMAN | EDITOR_ITEM_ORC, "Dark Portal");
   RADIO_ADD(EDITOR_ITEM_HUMAN | EDITOR_ITEM_ORC, "Runestone");

   elm_menu_item_separator_add(ed->menu, itm);

   i = elm_menu_item_add(ed->menu, itm, NULL, "NPC's", NULL, NULL);

   RADIO_ADD(EDITOR_ITEM_ORC, "Cho'Gall");
   RADIO_ADD(EDITOR_ITEM_ORC, "Zuljin");
   RADIO_ADD(EDITOR_ITEM_ORC, "Gul'Dan");
   RADIO_ADD(EDITOR_ITEM_ORC, "Grom Hellscream");
   RADIO_ADD(EDITOR_ITEM_ORC, "Kargath Bladefist");
   RADIO_ADD(EDITOR_ITEM_ORC, "Dentarg");
   RADIO_ADD(EDITOR_ITEM_ORC, "Teron Gorefiend");
   RADIO_ADD(EDITOR_ITEM_ORC, "Deathwing");

   elm_menu_item_separator_add(ed->menu, i);

   RADIO_ADD(EDITOR_ITEM_HUMAN, "Lothar");
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Uther Lightbringer");
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Turalyon");
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Alleria");
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Danath");
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Khadgar");
   RADIO_ADD(EDITOR_ITEM_HUMAN, "Kurdan and Sky'Ree");

   elm_menu_item_separator_add(ed->menu, i);

   RADIO_ADD(EDITOR_ITEM_HUMAN | EDITOR_ITEM_ORC, "Skeleton");
   RADIO_ADD(EDITOR_ITEM_HUMAN | EDITOR_ITEM_ORC, "Daemon");

#undef RADIO_ADD

   itm = elm_menu_item_add(ed->menu, NULL, NULL, "Players", NULL, NULL);
   itm = elm_menu_item_add(ed->menu, NULL, NULL, "Help", NULL, NULL);

   /* Show window */
   evas_object_show(ed->win);

   /* Open PUD */
   ed->pud = pud_open(ptr, PUD_OPEN_MODE_WRITE_ONLY);
   EINA_SAFETY_ON_NULL_GOTO(ed->pud, err_win_del);

   /* Add to list of editor windows */
   _windows = eina_list_append(_windows, ed);

   return ed;

err_win_del:
   evas_object_del(ed->win);
err_free:
   free(ed);
err_ret:
   return NULL;
}

