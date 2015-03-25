#include "war2edit.h"
#include <elm_interface_fileselector.h>

/*============================================================================*
 *                                 Private API                                *
 *============================================================================*/

static void
_fs_show(Editor *ed)
{
   elm_win_inwin_content_set(ed->inwin, ed->fs);
   elm_win_inwin_activate(ed->inwin);
   evas_object_show(ed->inwin);
}

static void
_fs_hide(Editor *ed)
{
   evas_object_hide(ed->inwin);
}


/*============================================================================*
 *                                  Callbacks                                 *
 *============================================================================*/

static void
_done_cb(void        *data,
         Evas_Object *obj   EINA_UNUSED,
         void        *event)
{
   Editor *ed = data;
   Eina_Stringshare *file = event;

   if (!file) goto hide_fileselector;

   if (ecore_file_is_dir(file))
     {
        ERR("You have selected a directory dumbass!");
        return;
     }

   ed->save_file = eina_stringshare_ref(event);
   EINA_SAFETY_ON_NULL_GOTO(ed->save_file, hide_fileselector);
   INF("Saving at path \"%s\"...", event);
   file_save(ed);


hide_fileselector:
   _fs_hide(ed);
}


/*============================================================================*
 *                                 Public API                                 *
 *============================================================================*/

Eina_Bool
file_selector_add(Editor *ed)
{
   Evas_Object *obj;

   obj = elm_fileselector_add(ed->win);
   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, EINA_FALSE);

   eo_do(
      obj,
      elm_interface_fileselector_folder_only_set(EINA_FALSE),
      elm_interface_fileselector_hidden_visible_set(EINA_FALSE),
      elm_interface_fileselector_sort_method_set(ELM_FILESELECTOR_SORT_BY_FILENAME_ASC),
      elm_interface_fileselector_multi_select_set(EINA_FALSE),
      elm_interface_fileselector_expandable_set(EINA_TRUE),
      elm_interface_fileselector_mode_set(ELM_FILESELECTOR_LIST),
      elm_obj_fileselector_buttons_ok_cancel_set(EINA_TRUE),
      evas_obj_size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND),
      evas_obj_size_hint_align_set(EVAS_HINT_FILL, EVAS_HINT_FILL)
   );
   evas_object_smart_callback_add(obj, "done", _done_cb, ed);

   ed->fs = obj;

   return EINA_TRUE;
}

Eina_Bool
file_save_prompt(Editor *ed)
{
   eo_do(ed->fs, elm_interface_fileselector_is_save_set(EINA_TRUE));
   _fs_show(ed);
   return EINA_TRUE;
}

Eina_Bool
file_load_prompt(Editor *ed)
{
   eo_do(ed->fs, elm_interface_fileselector_is_save_set(EINA_FALSE));
   _fs_show(ed);
   return EINA_TRUE;
}

Eina_Bool
file_save(Editor *ed)
{
   Eina_Bool chk;
   char description[32];
   int x, y, k = 0;
   struct _unit *u;
   Cell c;
   memset(description, 0, sizeof(description));

   /* Open PUD */
   if (!ed->pud)
     {
        ed->pud = pud_open(ed->save_file, PUD_OPEN_MODE_W);
        if (EINA_UNLIKELY(ed->pud == NULL))
          {
             CRI("Failed to create PUD file for \"%s\"", ed->save_file);
             goto panic;
          }
        chk = pud_defaults_set(ed->pud);
        if (EINA_UNLIKELY(chk == EINA_FALSE))
          {
             CRI("Failed to set defaults for PUD file \"%s\"", ed->save_file);
             goto panic;
          }
        /* Will never change, so set it once and for all */
        pud_dimensions_set(ed->pud, ed->size);
        pud_version_set(ed->pud, (ed->has_extension) ? 0x13 : 0x11);
        pud_tag_set(ed->pud, rand() % UINT32_MAX);
     }
   strncpy(description, "No description available", sizeof(description));
   pud_description_set(ed->pud, description);
   pud_era_set(ed->pud, ed->era);

   /* Set units count and allocate units */
   ed->pud->units_count = ed->units_count;
   ed->pud->units = realloc(ed->pud->units, ed->pud->units_count * sizeof((*ed->pud->units)));
   if (EINA_UNLIKELY(ed->pud->units == NULL))
      {
         CRI("Failed to allocate memory!!");
         goto panic;
      }

   // XXX owner ?
   for (x = 0; x < 8; x++)
     {
        if (ed->start_locations[x].x >= 0)
          ed->pud->starting_points++;
        ed->pud->side.players[x] = ed->sides[x];
     }
   /* FIXME This is by default. Needs to be implemented */
   ed->pud->human_players = 1;
   ed->pud->computer_players = 1;


   for (y = 0; y < ed->map_h; y++)
     {
        for (x = 0; x < ed->map_w; x++)
          {
             c = ed->cells[y][x];

             if (c.unit_below != PUD_UNIT_NONE)
               {
                  u = &(ed->pud->units[k++]);
                  u->x = x;
                  u->y = y;
                  u->type = c.unit_below;
                  u->owner = c.player_below;
                  u->alter = c.alter;
               }
             if (c.unit_above != PUD_UNIT_NONE)
               {
                  u = &(ed->pud->units[k++]);
                  u->x = x;
                  u->y = y;
                  u->type = c.unit_above;
                  u->owner = c.player_above;
                  u->alter = c.alter;
               }
             // FIXME c.tile is wrong
             pud_tile_set(ed->pud, x, y, c.tile);
          }
     }

   if (!pud_check(ed->pud))
     {
        CRI("Pud is not valid.");
        return EINA_FALSE;
     }

   chk = pud_write(ed->pud);
   if (EINA_UNLIKELY(chk == EINA_FALSE))
     {
        CRI("Failed to save pud!!");
        return EINA_FALSE;
     }

   return EINA_TRUE;

panic:
   pud_close(ed->pud);
   ed->pud = NULL;
   return EINA_FALSE;
}

