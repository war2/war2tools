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

   DBG("Segment %p changed: %i", obj, sd->val);
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
_icon_add(Evas_Object  *win,
          Evas_Object  *seg,
          const char   *file,
          void         *bind,
          unsigned int  value)
{
   Evas_Object *obj;
   Elm_Object_Item *eoi;
   Segment_Data *data;

   obj = elm_icon_add(win);
   EINA_SAFETY_ON_NULL_RETURN(obj);

   eo_do(
      obj,
      evas_obj_size_hint_aspect_set(EVAS_ASPECT_CONTROL_BOTH, 1, 1),
      elm_obj_image_resizable_set(EINA_TRUE, EINA_TRUE)
   );
   elm_image_file_set(obj, file, NULL);

   data = _segment_data_new(bind, value);
   EINA_SAFETY_ON_NULL_RETURN(data);

   eoi = elm_segment_control_item_add(seg, obj, NULL);
   EINA_SAFETY_ON_NULL_RETURN(eoi);
   elm_object_item_data_set(eoi, data);
}

static Evas_Object *
_segment_add(Editor      *ed,
             Evas_Object *box)
{
   Evas_Object *o;

   o = elm_segment_control_add(ed->win);
   eo_do(
      o,
      evas_obj_size_hint_weight_set(0.0, 0.0),
      evas_obj_size_hint_align_set(0.0, EVAS_HINT_FILL),
      evas_obj_visibility_set(EINA_TRUE)
   );
   evas_object_event_callback_add(o, EVAS_CALLBACK_DEL, _segment_free_cb, NULL);
   evas_object_smart_callback_add(o, "changed", _segment_changed_cb, ed);
   elm_box_pack_end(box, o);

   return o;
}

static void
_segment_constraint(Evas_Object *seg,
                    int          elems)
{
   /*
    * FIXME I think this is bad to use this.
    * TODO Investigate more into details.
    */

   const int w = elems * 30;
   const int h = 30;

   eo_do(
      seg,
      evas_obj_size_hint_max_set(w, h),
      evas_obj_size_hint_max_set(w, h)
   );
}


/*============================================================================*
 *                                 Public API                                 *
 *============================================================================*/

Eina_Bool
toolbar_add(Editor      *ed,
            Evas_Object *box)
{
   Evas_Object *seg[4], *obj;
   unsigned int i;
   Elm_Object_Item *eoi;

#define ITEM_ADD(seg_, win_, icon_, bind_, value_) \
   _icon_add(win_, seg_, DATA_DIR"/images/" icon_, bind_, value_)

   seg[0] = _segment_add(ed, box);
   ITEM_ADD(seg[0], ed->win, "light.png", &(ed->tint), EDITOR_TINT_LIGHT);
   ITEM_ADD(seg[0], ed->win, "dark.png", &(ed->tint), EDITOR_TINT_DARK);
   _segment_constraint(seg[0], 2);

   seg[1] = _segment_add(ed, box);
   ITEM_ADD(seg[1], ed->win, "spread_normal.png", &(ed->spread), EDITOR_SPREAD_NORMAL);
   ITEM_ADD(seg[1], ed->win, "spread_circle.png", &(ed->spread), EDITOR_SPREAD_CIRCLE);
   ITEM_ADD(seg[1], ed->win, "spread_random.png", &(ed->spread), EDITOR_SPREAD_RANDOM);
   _segment_constraint(seg[1], 3);

   seg[2] = _segment_add(ed, box);
   ITEM_ADD(seg[2], ed->win, "radius_small.png", &(ed->radius), EDITOR_RADIUS_SMALL);
   ITEM_ADD(seg[2], ed->win, "radius_medium.png", &(ed->radius), EDITOR_RADIUS_MEDIUM);
   ITEM_ADD(seg[2], ed->win, "radius_big.png", &(ed->radius), EDITOR_RADIUS_BIG);
   _segment_constraint(seg[2], 3);

   seg[3] = _segment_add(ed, box);
   ITEM_ADD(seg[3], ed->win, "magnifying_glass.png", &(ed->action), EDITOR_ACTION_SELECTION);
   ITEM_ADD(seg[3], ed->win, "water.png", &(ed->action), EDITOR_ACTION_WATER);
   ITEM_ADD(seg[3], ed->win, "mud.png", &(ed->action), EDITOR_ACTION_NON_CONSTRUCTIBLE);
   ITEM_ADD(seg[3], ed->win, "grass.png", &(ed->action), EDITOR_ACTION_CONSTRUCTIBLE);
   ITEM_ADD(seg[3], ed->win, "trees.png", &(ed->action), EDITOR_ACTION_TREES);
   ITEM_ADD(seg[3], ed->win, "rocks.png", &(ed->action), EDITOR_ACTION_ROCKS);
   ITEM_ADD(seg[3], ed->win, "human_wall.png", &(ed->action), EDITOR_ACTION_HUMAN_WALLS);
   ITEM_ADD(seg[3], ed->win, "orc_wall.png", &(ed->action), EDITOR_ACTION_ORCS_WALLS);
   _segment_constraint(seg[3], 8);

#undef ITEM_ADD

   /* Select an element */
   for (i = 0; i < EINA_C_ARRAY_LENGTH(seg); i++)
     {
        eoi = elm_segment_control_item_get(seg[i], 0);
        elm_segment_control_item_selected_set(eoi, EINA_TRUE);
     }

   /* Rectangle is used as a filler to force left-alignment */
   obj = evas_object_rectangle_add(evas_object_evas_get(ed->win));
   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, EINA_FALSE);
   eo_do(
      obj,
      evas_obj_color_set(0, 0, 0, 0),
      evas_obj_size_hint_weight_set(EVAS_HINT_EXPAND, 0.0),
      evas_obj_size_hint_align_set(1.0, 0.0),
      evas_obj_visibility_set(EINA_TRUE)
   );
   elm_box_pack_end(box, obj);

   return EINA_TRUE;
}

