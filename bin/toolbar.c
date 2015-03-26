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
_toolbar_free_cb(void        *data EINA_UNUSED,
                 Evas        *e    EINA_UNUSED,
                 Evas_Object *obj,
                 void        *info EINA_UNUSED)
{
   Elm_Object_Item *eoi;
   Segment_Data *sd;

   for (eoi = elm_toolbar_first_item_get(obj);
        eoi != NULL;
        eoi = elm_toolbar_item_next_get(eoi))
     {
        sd = elm_object_item_data_get(eoi);
        _segment_data_free(sd);
     }
}

static void
_item_active_cb(void        *data,
                Evas_Object *obj,
                void        *info)
{
   Editor *ed = data;
   Segment_Data *sd;
   Elm_Object_Item *eoi = info;

   sd = elm_object_item_data_get(eoi);

   *(sd->bind) = sd->val;

   /* Safely unset the unit selection */
   menu_unit_selection_reset(ed);

   DBG("Tb %p changed: %i", info, sd->val);
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

static Evas_Object *
_toolbar_add(Evas_Object *win,
             Evas_Object *box)
{
   Evas_Object *tb;

   tb = elm_toolbar_add(win);
   EINA_SAFETY_ON_NULL_RETURN_VAL(tb, NULL);

   eo_do(
      tb,
      elm_obj_toolbar_shrink_mode_set(ELM_TOOLBAR_SHRINK_MENU),
      evas_obj_size_hint_weight_set(EVAS_HINT_EXPAND, 0.0),
      evas_obj_size_hint_align_set(EVAS_HINT_FILL, 0.0),
      elm_obj_toolbar_homogeneous_set(EINA_FALSE),
      elm_obj_toolbar_align_set(0.0),
      evas_obj_visibility_set(EINA_TRUE)
   );

  // evas_object_event_callback_add(tb, EVAS_CALLBACK_DEL,
  //                                _toolbar_free_cb, NULL);

   elm_box_pack_end(box, tb);

   return tb;
}


/*============================================================================*
 *                                 Public API                                 *
 *============================================================================*/

Eina_Bool
toolbar_add(Editor      *ed,
            Evas_Object *box)
{
   Evas_Object *tb;
   Elm_Object_Item *eoi;
   Segment_Data *sd;

#define ITEM_ADD(tb_, icon_, name_, bind_, val_) \
   sd = _segment_data_new(bind_, val_); \
   eoi = elm_toolbar_item_append(tb_, DATA_DIR"/images/" icon_, name_, _item_active_cb, NULL); \
   elm_object_item_data_set(eoi, sd)

   tb = _toolbar_add(ed->win, box);
   ITEM_ADD(tb, "light.png", "Light", &(ed->tint), EDITOR_TINT_DARK);
   ITEM_ADD(tb, "dark.png", "Dark", &(ed->tint), EDITOR_TINT_LIGHT);

   tb = _toolbar_add(ed->win, box);
   ITEM_ADD(tb, "spread_normal.png", "Square", &(ed->spread), EDITOR_SPREAD_NORMAL);
   ITEM_ADD(tb, "spread_circle.png", "Circle", &(ed->spread), EDITOR_SPREAD_CIRCLE);
   ITEM_ADD(tb, "spread_random.png", "Random", &(ed->spread), EDITOR_SPREAD_RANDOM);

   tb = _toolbar_add(ed->win, box);
   ITEM_ADD(tb, "radius_small.png", "Small", &(ed->radius), EDITOR_RADIUS_SMALL);
   ITEM_ADD(tb, "radius_medium.png", "Medium", &(ed->radius), EDITOR_RADIUS_MEDIUM);
   ITEM_ADD(tb, "radius_big.png", "Big", &(ed->radius), EDITOR_RADIUS_BIG);

   tb = _toolbar_add(ed->win, box);
   ITEM_ADD(tb, "magnifying_glass.png", "Selection", &(ed->action), EDITOR_ACTION_SELECTION);
   ITEM_ADD(tb, "water.png", "Water", &(ed->action), EDITOR_ACTION_WATER);
   ITEM_ADD(tb, "mud.png", "Coast", &(ed->action), EDITOR_ACTION_NON_CONSTRUCTIBLE);
   ITEM_ADD(tb, "grass.png", "Grass", &(ed->action), EDITOR_ACTION_CONSTRUCTIBLE);
   ITEM_ADD(tb, "rocks.png", "Rocks", &(ed->action), EDITOR_ACTION_ROCKS);
   ITEM_ADD(tb, "human_wall.png", "Human Wall", &(ed->action), EDITOR_ACTION_HUMAN_WALLS);
   ITEM_ADD(tb, "orc_wall.png", "Orc Wall", &(ed->action), EDITOR_ACTION_ORCS_WALLS);


#undef ITEM_ADD

   return EINA_TRUE;
}

