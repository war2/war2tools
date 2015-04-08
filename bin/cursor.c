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
   ed->cursor.enabled = EINA_TRUE;
   ed->cursor.obj = obj;
   evas_object_show(obj);
   cursor_size_set(ed, 1, 1);
   cursor_pos_set(ed, 0, 0);

   return EINA_TRUE;
}

void
cursor_enable(Editor *ed)
{
   if (!ed->cursor.enabled)
     {
        edje_object_signal_emit(ed->cursor.obj, "enable", "war2edit");
        ed->cursor.enabled = EINA_TRUE;
     }
}

void
cursor_disable(Editor *ed)
{
   if (ed->cursor.enabled)
     {
        edje_object_signal_emit(ed->cursor.obj, "disable", "war2edit");
        ed->cursor.enabled = EINA_FALSE;
     }
}

void
cursor_size_set(Editor *ed,
                int     w,
                int     h)
{
   if ((w != ed->cursor.w) || (h != ed->cursor.h))
     {
        evas_object_resize(ed->cursor.obj, w * 32, h * 32);
        ed->cursor.w = w;
        ed->cursor.h = h;
     }
}

void
cursor_size_get(Editor *ed,
                int    *w,
                int    *h)
{
   if (w) *w = ed->cursor.w;
   if (h) *h = ed->cursor.h;
}

void
cursor_pos_set(Editor *ed,
               int     x,
               int     y)
{
   int bx, by;

   if ((ed->cursor.x != x) || (ed->cursor.y != y))
     {
        eo_do(ed->bitmap, efl_gfx_position_get(&bx, &by));
        evas_object_move(ed->cursor.obj, bx + x * 32, by + y * 32);
        ed->cursor.x = x;
        ed->cursor.y = y;
     }
}

void
cursor_pos_get(Editor *ed,
               int    *x,
               int    *y)
{
   if (x) *x = ed->cursor.x;
   if (y) *y = ed->cursor.y;
}


void
cursor_hide(Editor *ed)
{
   edje_object_signal_emit(ed->cursor.obj, "hide", "war2edit");
}

void
cursor_show(Editor *ed)
{
   edje_object_signal_emit(ed->cursor.obj, "show", "war2edit");
}

