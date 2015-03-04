#include "war2edit.h"

static const char EDITOR_DATA_KEY[] = "war2/editor";


/*============================================================================*
 *                                 Private API                                *
 *============================================================================*/

static Grid_Cell **
_grid_cells_new(Editor *ed)
{
   Grid_Cell **ptr;
   int i;

   /* Iliffe vector allocation */

   ptr = malloc(ed->map_h * sizeof(*ptr));
   EINA_SAFETY_ON_NULL_RETURN_VAL(ptr, NULL);

   ptr[0] = calloc(ed->map_w * ed->map_h, sizeof(**ptr));
   EINA_SAFETY_ON_NULL_GOTO(ptr[0], fail);

   for (i = 1; i < ed->map_h; ++i)
     ptr[i] = ptr[i - 1] + ed->map_w;

   return ptr;

fail:
   free(ptr);
   return NULL;
}

static inline Editor *
_editor_get(Evas_Object *glv)
{
   return evas_object_data_get(glv, EDITOR_DATA_KEY);
}


/*============================================================================*
 *                              OpenGL Callbacks                              *
 *============================================================================*/

static void
_init_gl(Evas_Object *glv)
{
}

static void
_del_gl(Evas_Object *glv)
{
}

static void
_resize_gl(Evas_Object *glv)
{
}

static void
_draw_gl(Evas_Object *glv)
{
}


/*============================================================================*
 *                                 Public API                                 *
 *============================================================================*/

Eina_Bool
grid_add(Editor *ed)
{
   Evas_Object *glv;

   glv = elm_glview_add(ed->win);
   EINA_SAFETY_ON_NULL_RETURN_VAL(glv, EINA_FALSE);

   evas_object_size_hint_align_set(glv, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(glv, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

   elm_glview_mode_set(glv, ELM_GLVIEW_ALPHA | ELM_GLVIEW_DEPTH);
   elm_glview_resize_policy_set(glv, ELM_GLVIEW_RESIZE_POLICY_RECREATE);
   elm_glview_render_policy_set(glv, ELM_GLVIEW_RENDER_POLICY_ON_DEMAND);
   elm_glview_init_func_set(glv, _init_gl);
   elm_glview_del_func_set(glv, _del_gl);
   elm_glview_resize_func_set(glv, _resize_gl);
   elm_glview_render_func_set(glv, _draw_gl);

   evas_object_data_set(glv, EDITOR_DATA_KEY, ed);

   ed->cells = _grid_cells_new(ed);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ed->cells, EINA_FALSE);

   ed->glview = glv;
   ed->gl = elm_glview_gl_api_get(glv);

   return EINA_TRUE;
}

