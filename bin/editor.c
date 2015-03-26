#include "war2edit.h"

static Eina_List *_editors = NULL;


/*============================================================================*
 *                                  Callbacks                                 *
 *============================================================================*/


static void
_win_focused_cb(void        *data  EINA_UNUSED,
                Evas_Object *obj   EINA_UNUSED,
                void        *event EINA_UNUSED)
{
//   Editor *ed = data;
}

static void
_win_del_cb(void        *data,
            Evas_Object *obj   EINA_UNUSED,
            void        *event EINA_UNUSED)
{
   Editor *ed = data;
   editor_free(ed);
}

//static void
//_menu_clicked_cb(void        *data,
//                 Evas_Object *obj   EINA_UNUSED,
//                 void        *event EINA_UNUSED)
//{
//   Editor *ed = data;
//   printf("Clicked\n");
//   cursor_hide(ed);
//}
//
//static void
//_menu_dismissed_cb(void        *data,
//                   Evas_Object *obj   EINA_UNUSED,
//                   void        *event EINA_UNUSED)
//{
//   Editor *ed = data;
//   printf("Dismissed\n");
//   cursor_show(ed);
//}

static void
_error_close_cb(void        *data,
                Evas_Object *obj  EINA_UNUSED,
                void        *info EINA_UNUSED)
{
   Editor *ed = data;
   DBG("Closing Editor %p after error...", ed);
   editor_free(ed);
}


/*============================================================================*
 *                                 Public API                                 *
 *============================================================================*/

Eina_Bool
editor_init(void)
{
   return EINA_TRUE;
}

void
editor_shutdown(void)
{
   Editor *ed;

   EINA_LIST_FREE(_editors, ed)
      editor_free(ed);
}

void
editor_free(Editor *ed)
{
   EINA_SAFETY_ON_NULL_RETURN(ed);

   _editors = eina_list_remove(_editors, ed);
   if (ed->textures) eina_hash_free(ed->textures);
   if (ed->textures_src) eet_close(ed->textures_src);
   if (ed->units) eet_close(ed->units);
   if (ed->buildings) eet_close(ed->buildings);
   if (ed->sprites) eina_hash_free(ed->sprites);
   if (ed->save_file) eina_stringshare_del(ed->save_file);
   free(ed->pixels);
   cell_matrix_free(ed->cells);
   pud_close(ed->pud);
   evas_object_del(ed->win);
   free(ed);
}

void
editor_error(Editor     *ed,
             const char *msg)
{
   Evas_Object *box, *o, *e;

   /* Confirm button */
   o = elm_button_add(ed->win);
   EINA_SAFETY_ON_NULL_GOTO(o, end);
   elm_object_text_set(o, "Ok");
   evas_object_smart_callback_add(o, "clicked", _error_close_cb, ed);

   /* Info label */
   e = elm_label_add(ed->win);
   EINA_SAFETY_ON_NULL_GOTO(e, end);
   elm_object_text_set(e, msg);
   eo_do(
      e,
      evas_obj_size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND),
      evas_obj_size_hint_align_set(EVAS_HINT_FILL, EVAS_HINT_FILL)
   );

   /* Box to content the UI */
   box = elm_box_add(ed->win);
   EINA_SAFETY_ON_NULL_GOTO(box, end);
   eo_do(
      box,
      evas_obj_size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND),
      evas_obj_size_hint_align_set(EVAS_HINT_FILL, EVAS_HINT_FILL),
      elm_obj_box_horizontal_set(EINA_FALSE),
      elm_obj_box_homogeneous_set(EINA_FALSE),
      elm_obj_box_pack_start(e),
      elm_obj_box_pack_end(o)
   );

   elm_win_inwin_content_set(ed->inwin, box);
   elm_win_inwin_activate(ed->inwin);
   evas_object_show(ed->inwin);
   evas_object_show(box);
   evas_object_show(o);
   evas_object_show(e);

   return;
end:
   CRI("Failed to generate error UI with message [%s]", msg);
   editor_free(ed);
}


Editor *
editor_new(void)
{
   Editor *ed;
   char title[128], wins[32];
   Evas_Object *o, *box;
   int i;

   ed = calloc(1, sizeof(Editor));
   EINA_SAFETY_ON_NULL_GOTO(ed, err_ret);

   for (i = 0; i < 8; i++)
     {
        /* No start location */
        ed->start_locations[i].x = -1;
        ed->start_locations[i].y = -1;

        /* Set initial races */
        if (i % 2 == 0) ed->sides[i] = PUD_SIDE_HUMAN;
        else ed->sides[i] = PUD_SIDE_ORC;
     }


   /* Create window's title */
   snprintf(wins, sizeof(wins), " - %i", eina_list_count(_editors));
   snprintf(title, sizeof(title), "Untitled%s",
            (eina_list_count(_editors) == 0) ? "" : wins);

   /* Create window and set callbacks */
   ed->win = elm_win_util_standard_add("win-editor", title);
   EINA_SAFETY_ON_NULL_GOTO(ed->win, err_free);
   elm_win_focus_highlight_enabled_set(ed->win, EINA_FALSE);
   evas_object_smart_callback_add(ed->win, "focus,in", _win_focused_cb, ed);
   evas_object_smart_callback_add(ed->win, "delete,request", _win_del_cb, ed);
   evas_object_resize(ed->win, 640, 480);

   /* File selector */
   file_selector_add(ed);

   /* Add a box to put widgets in it */
   o = elm_box_add(ed->win);
   EINA_SAFETY_ON_NULL_GOTO(o, err_win_del);
   ed->mainbox = o;
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_horizontal_set(o, EINA_FALSE);
   elm_win_resize_object_add(ed->win, o);
   evas_object_show(o);

   /* Get the main menu */
   menu_add(ed);

   /* Toolbar */
   box = elm_box_add(ed->win);
   EINA_SAFETY_ON_NULL_GOTO(box, err_win_del);
   eo_do(
         box,
         evas_obj_size_hint_weight_set(EVAS_HINT_EXPAND, 0.0),
         evas_obj_size_hint_align_set(EVAS_HINT_FILL, EVAS_HINT_FILL);
         elm_obj_box_horizontal_set(EINA_TRUE),
         elm_obj_box_homogeneous_set(EINA_FALSE),
         evas_obj_visibility_set(EINA_TRUE)
      );
   elm_box_pack_end(o, box);
   toolbar_add(ed, box);


   /* Scroller */
   ed->scroller = elm_scroller_add(ed->win);
   EINA_SAFETY_ON_NULL_GOTO(ed->scroller, err_win_del);
   evas_object_size_hint_weight_set(ed->scroller, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(ed->scroller, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_scroller_policy_set(ed->scroller, ELM_SCROLLER_POLICY_ON, ELM_SCROLLER_POLICY_ON);
   /* elm_scroller_propagate_events_set(ed->scroller, EINA_FALSE); */
   elm_box_pack_end(o, ed->scroller);
   evas_object_show(ed->scroller);
   elm_scroller_page_relative_set(ed->scroller, 0, 1);

   /* Mainconfig: get user input for various mainstream parameters */
   mainconfig_add(ed);

   /* Add inwin */
   ed->inwin = elm_win_inwin_add(ed->win);
   EINA_SAFETY_ON_NULL_GOTO(ed->inwin, err_win_del);

   /* Show window */
   evas_object_show(ed->win);

   mainconfig_show(ed);

   /* Add to list of editor windows */
   _editors = eina_list_append(_editors, ed);

   return ed;

err_win_del:
   evas_object_del(ed->win);
err_free:
   free(ed);
err_ret:
   return NULL;
}

void
editor_finalize(Editor *ed)
{
   int x, y;
   Eina_Bool chk;

   pud_dimensions_to_size(ed->size, &(ed->map_w), &(ed->map_h));

   snprintf(ed->era_str, sizeof(ed->era_str), "%s", pud_era2str(ed->era));
   ed->era_str[0] += 32; /* Lowercase */

   menu_unit_selection_reset(ed);

   ed->units = sprite_units_open();
   ed->buildings = sprite_buildings_open(ed->era);
   ed->sprites = sprite_hash_new();

   ed->textures_src = texture_tileset_open(ed->era);
   ed->textures = texture_hash_new();

   chk = bitmap_add(ed);
   EINA_SAFETY_ON_FALSE_RETURN(chk);
   elm_object_content_set(ed->scroller, ed->bitmap);
   evas_object_show(ed->bitmap);

   /* Get the origin of the bitmap. This is done once
    * and is used to get the origin of the clipping view */
   evas_object_geometry_get(ed->bitmap, &x, &y, NULL, NULL);
   ed->bitmap_origin.x = x;
   ed->bitmap_origin.y = y;

   cursor_add(ed);
}

