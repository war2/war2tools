#include "war2edit.h"

typedef struct
{
   unsigned int *bind;
   unsigned int  val;
} Segment_Data;

static Segment_Data *_segment_data_new(unsigned int *bind, unsigned int val);
static void _segment_data_free(Segment_Data *data);


/*============================================================================*
 *                                  Callbacks                                 *
 *============================================================================*/

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
_segment_changed_cb(void        *data,
                    Evas_Object *obj,
                    void        *info EINA_UNUSED)
{
   Editor *ed = data;
   Segment_Data *sd;
   Elm_Object_Item *eoi;

   eoi = elm_segment_control_item_selected_get(obj);
   sd = elm_object_item_data_get(eoi);

   *(sd->bind) = sd->val;

   /* Safely unset the unit selection */
   menu_unit_selection_reset(ed);

   DBG("Clicked on segment %p: %i", obj, sd->val);
}


/*============================================================================*
 *                                 Private API                                *
 *============================================================================*/

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


/*============================================================================*
 *                                 Public API                                 *
 *============================================================================*/

void
toolbar_fill(Editor      *ed,
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
        evas_object_smart_callback_add(seg, "changed", _segment_changed_cb, ed);
        eoi = elm_segment_control_item_get(seg, 0);
        elm_segment_control_item_selected_set(eoi, EINA_TRUE);
     }
}

