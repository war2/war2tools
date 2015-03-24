#include "war2edit.h"

/*============================================================================*
 *                                  Callbacks                                 *
 *============================================================================*/

static void
_mc_cancel_cb(void        *data,
              Evas_Object *obj  EINA_UNUSED,
              void        *evt  EINA_UNUSED)
{
   Editor *ed = data;
   editor_free(ed);
}

static void
_mc_create_cb(void        *data,
              Evas_Object *obj  EINA_UNUSED,
              void        *evt  EINA_UNUSED)
{
   Editor *ed = data;
   mainconfig_hide(ed);
   editor_finalize(ed);
}

static void
_size_changed_cb(void        *data,
                 Evas_Object *obj,
                 void        *event_info EINA_UNUSED)
{
   Editor *ed = data;
   int id;

   id = elm_radio_value_get(obj);
   switch (id)
     {
      case 1: ed->size = PUD_DIMENSIONS_32_32;   break;
      case 2: ed->size = PUD_DIMENSIONS_64_64;   break;
      case 3: ed->size = PUD_DIMENSIONS_96_96;   break;
      case 4: ed->size = PUD_DIMENSIONS_128_128; break;

      default:
              CRI("Invalid ID for size radio group [%i]", id);
              break;
     }
}

static void
_era_changed_cb(void        *data,
                Evas_Object *obj,
                void        *event_info EINA_UNUSED)
{
   Editor *ed = data;
   int id;

   id = elm_radio_value_get(obj);
   switch (id)
     {
      case 1: ed->era = PUD_ERA_FOREST;    break;
      case 2: ed->era = PUD_ERA_WINTER;    break;
      case 3: ed->era = PUD_ERA_WASTELAND; break;
      case 4: ed->era = PUD_ERA_SWAMP;     break;

      default:
              CRI("Invalid ID for era radio group [%i]", id);
              break;
     }
}

static void
_has_extension_cb(void        *data,
                  Evas_Object *obj,
                  void        *event_info EINA_UNUSED)
{
   Editor *ed = data;
   Eina_Bool state;

   state = elm_check_state_get(obj);

   // TODO Disable/Enable menu
   if (state == EINA_FALSE) {}
   else {}

   ed->has_extension = state;
}


/*============================================================================*
 *                                 Public API                                 *
 *============================================================================*/

Eina_Bool
mainconfig_add(Editor *ed)
{
   Evas_Object *o, *box, *b2, *b3, *img, *f, *b, *grp;

   /* Create main box (mainconfig) */
   box = elm_box_add(ed->win);
   evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_horizontal_set(box, EINA_FALSE);

   /* Box to hold buttons */
   b2 = elm_box_add(box);
   elm_box_horizontal_set(b2, EINA_TRUE);
   evas_object_size_hint_weight_set(b2, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_box_pack_end(box, b2);
   evas_object_show(b2);

   /* Cancel button */
   o = elm_button_add(b2);
   elm_object_text_set(o, "Cancel");
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_smart_callback_add(o, "clicked", _mc_cancel_cb, ed);
   elm_box_pack_start(b2, o);
   evas_object_show(o);

   /* Create button */
   o = elm_button_add(b2);
   elm_object_text_set(o, "Create");
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_smart_callback_add(o, "clicked", _mc_create_cb, ed);
   elm_box_pack_end(b2, o);
   evas_object_show(o);

   /* Box to hold map and menus */
   b2 = elm_box_add(box);
   elm_box_horizontal_set(b2, EINA_TRUE);
   evas_object_size_hint_weight_set(b2, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_box_pack_start(box, b2);
   evas_object_show(b2);

   /* Extension checker */
   o = elm_check_add(box);
   elm_object_text_set(o, "Use Warcraft II extension - Beyond the Dark Portal");
   // FIXME Remove the disabled when cb will be implemented
   elm_check_state_set(o, EINA_TRUE);
   elm_object_disabled_set(o, EINA_TRUE);
   evas_object_smart_callback_add(o, "changed", _has_extension_cb, ed);
   elm_box_pack_start(box, o);
   evas_object_show(o);

   /* Image to hold the minimap overview */
   img = elm_image_add(b2);
   elm_box_pack_start(b2, img);
   //   elm_image_file_set(img,

   /* Box to put commands */
   b3 = elm_box_add(b2);
   elm_box_horizontal_set(b3, EINA_FALSE);
   evas_object_size_hint_weight_set(b3, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_box_pack_end(b2, b3);
   evas_object_show(b3);

   /* Frame for map size */
   f = elm_frame_add(b3);
   elm_object_text_set(f, "Map Size");
   evas_object_size_hint_weight_set(f, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(f, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_start(b3, f);
   evas_object_show(f);
   b = elm_box_add(f); /* Box */
   elm_object_content_set(f, b);
   elm_box_align_set(b, 0.0f, 0.0f);
   evas_object_show(b);
   o = elm_radio_add(b); /* Size item 1 */
   elm_radio_state_value_set(o, 1);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, 0.5);
   elm_object_text_set(o, "32 x 32");
   elm_box_pack_end(b, o);
   evas_object_show(o);
   grp = o;
   evas_object_smart_callback_add(o, "changed", _size_changed_cb, ed);
   o = elm_radio_add(b); /* Size item 2 */
   elm_radio_state_value_set(o, 2);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, 0.5);
   elm_object_text_set(o, "64 x 64");
   elm_box_pack_end(b, o);
   evas_object_show(o);
   elm_radio_group_add(o, grp);
   evas_object_smart_callback_add(o, "changed", _size_changed_cb, ed);
   o = elm_radio_add(b); /* Size item 3 */
   elm_radio_state_value_set(o, 3);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, 0.5);
   elm_object_text_set(o, "96 x 96");
   elm_box_pack_end(b, o);
   evas_object_show(o);
   elm_radio_group_add(o, grp);
   evas_object_smart_callback_add(o, "changed", _size_changed_cb, ed);
   o = elm_radio_add(b); /* Size item 4 */
   elm_radio_state_value_set(o, 4);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, 0.5);
   elm_object_text_set(o, "128 x 128");
   elm_box_pack_end(b, o);
   evas_object_show(o);
   elm_radio_group_add(o, grp);
   evas_object_smart_callback_add(o, "changed", _size_changed_cb, ed);
   elm_radio_value_set(grp, 1);
   ed->size = PUD_DIMENSIONS_32_32;
   ed->mainconfig.menu_size = grp;

   /* Frame for map era */
   f = elm_frame_add(b3);
   elm_object_text_set(f, "Tileset");
   evas_object_size_hint_weight_set(f, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(f, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(b3, f);
   evas_object_show(f);
   b = elm_box_add(f); /* Box */
   elm_object_content_set(f, b);
   elm_box_align_set(b, 0.0f, 0.0f);
   evas_object_show(b);
   o = elm_radio_add(b); /* Tileset item 1 */
   elm_radio_state_value_set(o, 1);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, 0.5);
   elm_object_text_set(o, "Forest");
   elm_box_pack_end(b, o);
   evas_object_show(o);
   grp = o;
   evas_object_smart_callback_add(o, "changed", _era_changed_cb, ed);
   o = elm_radio_add(b); /* Tileset item 2 */
   elm_radio_state_value_set(o, 2);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, 0.5);
   elm_object_text_set(o, "Winter");
   elm_box_pack_end(b, o);
   evas_object_show(o);
   elm_radio_group_add(o, grp);
   evas_object_smart_callback_add(o, "changed", _era_changed_cb, ed);
   o = elm_radio_add(b); /* Tileset item 3 */
   elm_radio_state_value_set(o, 3);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, 0.5);
   elm_object_text_set(o, "Wasteland");
   elm_box_pack_end(b, o);
   evas_object_show(o);
   elm_radio_group_add(o, grp);
   evas_object_smart_callback_add(o, "changed", _era_changed_cb, ed);
   o = elm_radio_add(b); /* Tileset item 4 */
   elm_radio_state_value_set(o, 4);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, 0.5);
   elm_object_text_set(o, "Swamp");
   elm_box_pack_end(b, o);
   evas_object_show(o);
   evas_object_smart_callback_add(o, "changed", _era_changed_cb, ed);
   elm_radio_group_add(o, grp);
   elm_radio_value_set(grp, 1);
   ed->era = PUD_ERA_FOREST;
   ed->mainconfig.menu_era = grp;

   ed->mainconfig.container = box;
   ed->mainconfig.img = img;

   return EINA_TRUE;
}

void
mainconfig_show(Editor *ed)
{
   int i;

   /* Disable main menu */
   for (i = 0; i < 4; i++)
     elm_object_item_disabled_set(ed->main_sel[i], EINA_TRUE);

   /* Show inwin */
   elm_win_inwin_activate(ed->inwin);
   evas_object_show(ed->inwin);
}

void
mainconfig_hide(Editor *ed)
{
   int i;

   evas_object_hide(ed->inwin);
   for (i = 0; i < 4; i++)
     elm_object_item_disabled_set(ed->main_sel[i], EINA_FALSE);
}

