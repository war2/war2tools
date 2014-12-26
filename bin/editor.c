#include "war2edit.h"

static Eina_List *_windows;

Eina_Bool
editor_init(void)
{
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
   free(ed);
}

Editor *
editor_new(Pud_Era        era,
           Pud_Dimensions dims)
{
   Editor *ed;

   ed = calloc(1, sizeof(Editor));
   EINA_SAFETY_ON_NULL_RETURN_VAL(ed, NULL);

   _windows = eina_list_append(_windows, ed);

   return ed;
}

