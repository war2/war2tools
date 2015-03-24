#include "war2edit.h"

Eina_Bool
cursor_add(Editor *ed)
{
   Evas_Object *obj;
   const char *edje;
   Eina_Bool chk;

   obj = edje_object_add(evas_object_evas_get(ed->win));
   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, EINA_FALSE);

   evas_object_pass_events_set(obj, EINA_TRUE);
   evas_object_propagate_events_set(obj, EINA_FALSE);

   edje = DATA_DIR"/themes/default.edj";
   chk = edje_object_file_set(obj, edje, "war2edit/cursor");
   if (!chk)
     {
        CRI("Failed to set cursor from theme file [%s]", edje);
        return EINA_FALSE;
     }
   ed->cursor_is_enabled = EINA_TRUE;
   ed->cursor = obj;
   evas_object_show(obj);
   cursor_resize(ed, 1, 1);

   return EINA_TRUE;
}

void
cursor_enable(Editor *ed)
{
   if (!ed->cursor_is_enabled)
     {
        edje_object_signal_emit(ed->cursor, "enable", "");
        ed->cursor_is_enabled = EINA_TRUE;
     }
}

void
cursor_disable(Editor *ed)
{
   if (ed->cursor_is_enabled)
     {
        edje_object_signal_emit(ed->cursor, "disable", "war2edit");
        ed->cursor_is_enabled = EINA_FALSE;
     }
}

void
cursor_resize(Editor *ed,
              int     w,
              int     h)
{
   evas_object_resize(ed->cursor, w * 32, h * 32);
}

void
cursor_move(Editor *ed,
            int     x,
            int     y)
{
   int bx, by;

   eo_do(ed->bitmap, evas_obj_position_get(&bx, &by));
   evas_object_move(ed->cursor, bx + x * 32, by + y * 32);
}

