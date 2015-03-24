#include "war2edit.h"

typedef struct
{
   unsigned int *bind;
   unsigned int  val;
} Segment_Data;

static Eina_List *_editors = NULL;


static Segment_Data *
_segment_data_new(unsigned int *bind,
                  unsigned int  val)
{
   Segment_Data *data;

   data = malloc(sizeof(*data));
   EINA_SAFETY_ON_NULL_RETURN_VAL(data, NULL);

   data->bind = bind;
   data->val = val;

   return data;
}

static void
_segment_data_free(Segment_Data *data)
{
   free(data);
}


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

static void
_segment_free_cb(void        *data EINA_UNUSED,
                 Evas        *e    EINA_UNUSED,
                 Evas_Object *obj,
                 void        *info EINA_UNUSED)
{
   unsigned int i, count;
   Elm_Object_Item *eoi;
   Segment_Data *sd;

   count = elm_segment_control_item_count_get(obj);
   for (i = 0; i < count; i++)
     {
        eoi = elm_segment_control_item_get(obj, i);
        sd = elm_object_item_data_get(eoi);
        _segment_data_free(sd);
     }
}

static void
_item_add(Evas_Object  *seg,
          Evas_Object  *win,
          const char   *filename,
          void         *bind,
          unsigned int  value)
{
   char path[PATH_MAX];
   Evas_Object *o;
   Elm_Object_Item *eoi;
   Segment_Data *data;

   snprintf(path, sizeof(path), DATA_DIR"/images/%s", filename);
   o = elm_icon_add(win);
   elm_image_file_set(o, path, NULL);
   evas_object_size_hint_aspect_set(o, EVAS_ASPECT_CONTROL_BOTH, 1, 1);
   elm_image_resizable_set(o, EINA_TRUE, EINA_TRUE);

   data = _segment_data_new(bind, value);
   eoi = elm_segment_control_item_add(seg, o, NULL);
   elm_object_item_data_set(eoi, data);
}

static void
_object_set(Evas_Object *tb,
            Evas_Object *obj)
{
   Elm_Object_Item *eoi;
   eoi = elm_toolbar_item_append(tb, NULL, NULL, NULL, NULL);
   elm_object_item_part_content_set(eoi, "object", obj);
}

static Evas_Object *
_segment_add(Evas_Object *win)
{
   Evas_Object *o;

   o = elm_segment_control_add(win);
   eo_do(
      o,
      evas_obj_size_hint_weight_set(EVAS_HINT_EXPAND, 0.0),
      evas_obj_size_hint_align_set(EVAS_HINT_FILL, EVAS_HINT_FILL)
   );
   evas_object_event_callback_add(o, EVAS_CALLBACK_DEL, _segment_free_cb, NULL);

   return o;
}

static void
_segment_changed_cb(void        *data EINA_UNUSED,
                    Evas_Object *obj,
                    void        *info EINA_UNUSED)
{
   Segment_Data *sd;
   Elm_Object_Item *eoi;

   eoi = elm_segment_control_item_selected_get(obj);
   sd = elm_object_item_data_get(eoi);

   *(sd->bind) = sd->val;

   DBG("Clicked on segment %p: %i\n", obj, sd->val);
}

static void
_toolbar_fill(Editor      *ed,
              Evas_Object *tb,
              Evas_Object *win)
{
   Evas_Object *seg;
   unsigned int i;
   Elm_Object_Item *eoi;

   seg = _segment_add(win);
   _item_add(seg, win, "light.png", &(ed->tint), EDITOR_TINT_LIGHT);
   _item_add(seg, win, "dark.png", &(ed->tint), EDITOR_TINT_DARK);
   ed->segments[0] = seg;

   seg = _segment_add(win);
   _item_add(seg, win, "spread_normal.png", &(ed->spread), EDITOR_SPREAD_NORMAL);
   _item_add(seg, win, "spread_circle.png", &(ed->spread), EDITOR_SPREAD_CIRCLE);
   _item_add(seg, win, "spread_random.png", &(ed->spread), EDITOR_SPREAD_RANDOM);
   ed->segments[1] = seg;

   seg = _segment_add(win);
   _item_add(seg, win, "radius_small.png", &(ed->radius), EDITOR_RADIUS_SMALL);
   _item_add(seg, win, "radius_medium.png", &(ed->radius), EDITOR_RADIUS_MEDIUM);
   _item_add(seg, win, "radius_big.png", &(ed->radius), EDITOR_RADIUS_BIG);
   _object_set(tb, seg);
   ed->segments[2] = seg;

   seg = _segment_add(win);
   _item_add(seg, win, "magnifying_glass.png", &(ed->action), EDITOR_ACTION_SELECTION);
   _item_add(seg, win, "water.png", &(ed->action), EDITOR_ACTION_WATER);
   _item_add(seg, win, "mud.png", &(ed->action), EDITOR_ACTION_NON_CONSTRUCTIBLE);
   _item_add(seg, win, "grass.png", &(ed->action), EDITOR_ACTION_CONSTRUCTIBLE);
   _item_add(seg, win, "trees.png", &(ed->action), EDITOR_ACTION_TREES);
   _item_add(seg, win, "rocks.png", &(ed->action), EDITOR_ACTION_ROCKS);
   _item_add(seg, win, "human_wall.png", &(ed->action), EDITOR_ACTION_HUMAN_WALLS);
   _item_add(seg, win, "orc_wall.png", &(ed->action), EDITOR_ACTION_ORCS_WALLS);
   evas_object_size_hint_min_set(seg, 200, 20);  /* FIXME Hotfix FIXME */
   ed->segments[3] = seg;

   for (i = 0; i < EINA_C_ARRAY_LENGTH(ed->segments); i++)
     {
        seg = ed->segments[i];
        _object_set(tb, seg);
        evas_object_smart_callback_add(seg, "changed", _segment_changed_cb, NULL);
        eoi = elm_segment_control_item_get(seg, 0);
        elm_segment_control_item_selected_set(eoi, EINA_TRUE);
     }
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
   free(ed->pixels);
   if (ed->cells) free(ed->cells[0]);
   free(ed->cells);
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
   Evas_Object *o;
   Elm_Object_Item *itm, *i;
   int k = 0;

   ed = calloc(1, sizeof(Editor));
   EINA_SAFETY_ON_NULL_GOTO(ed, err_ret);

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
   ed->toolbar = elm_toolbar_add(ed->win);
   EINA_SAFETY_ON_NULL_GOTO(ed->toolbar, err_win_del);
   eo_do(
      ed->toolbar,
      elm_obj_toolbar_shrink_mode_set(ELM_TOOLBAR_SHRINK_SCROLL),
      evas_obj_size_hint_weight_set(EVAS_HINT_EXPAND, 0.0),
      evas_obj_size_hint_align_set(EVAS_HINT_FILL, 0.0),
      elm_obj_toolbar_homogeneous_set(EINA_FALSE),
      elm_obj_toolbar_align_set(0.0),
      evas_obj_visibility_set(EINA_TRUE)
   );

   elm_toolbar_transverse_expanded_set(ed->toolbar, EINA_TRUE);
   _toolbar_fill(ed, ed->toolbar, ed->win);
   elm_box_pack_end(o, ed->toolbar);

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
   elm_win_inwin_content_set(ed->inwin, ed->mainconfig.container);

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

   ed->units = sprite_units_open();
   ed->buildings = sprite_buildings_open(ed->era);
   ed->sprites = sprite_hash_new();

   ed->textures_src = texture_tileset_open(ed->era);
   texture_dictionary_init(&(ed->tex_dict), ed->era);
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
   cursor_move(ed, 0, 0);
}

