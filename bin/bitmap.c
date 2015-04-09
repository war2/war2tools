#include "war2edit.h"

typedef struct {
   Editor    *ed;
   int        colorize;
   Eina_Bool  hflip;
} Draw_Data;

/*============================================================================*
 *                                 Private API                                *
 *============================================================================*/

static void
_bitmap_draw_func(void                 *data,
                  Evas_Object          *obj  EINA_UNUSED,
                  Elm_Bitmap_Draw_Info *info)
{
   Draw_Data *d = data;
   const int bmp_w = info->bitmap_w * 4;
   int img_y, bmp_y, img_x, bmp_x, k;
   int img_w, at_x;
   unsigned char *restrict bmp = info->pixels;
   unsigned char bgr[4];
   int bmp_x_start;
   int bmp_x_step;

   img_w = info->source_w * 4;
   at_x = info->at_x * 4;

   /* Always used when there is full alpha */
   for (img_y = 0, bmp_y = info->at_y;
        (img_y < info->source_h) && (bmp_y < info->bitmap_h);
        ++img_y, ++bmp_y)
     {
        /* Calculate the horizontal mirroring  */
        if (d->hflip)
          {
             bmp_x_start = at_x + img_w;
             bmp_x_step = -4;
          }
        else
          {
             bmp_x_start = at_x;
             bmp_x_step = 4;
          }

        for (img_x = 0, bmp_x = bmp_x_start;
             (img_x < img_w) && (bmp_x < bmp_w);
             img_x += 4, bmp_x += bmp_x_step)
          {
             k = img_x + (img_y * img_w);
             memcpy(&(bgr[0]), &(info->source[k]), 4);
             if (d->colorize != -1)
               {
                  war2_sprites_color_convert(d->colorize,
                                             bgr[2], bgr[1], bgr[0],
                                             &(bgr[2]), &(bgr[1]), &(bgr[0]));
               }
             if (bgr[3] != 0)
               {
                  memcpy(&(bmp[bmp_x + bmp_y * bmp_w]), &(bgr[0]), 4);
               }
          }
     }
}

static void
_draw(Editor *ed,
      unsigned char *restrict img,
      int                     at_x,
      int                     at_y,
      int                     img_w,
      int                     img_h,
      Eina_Bool               hflip,
      int                     colorize)
{
   Draw_Data *draw_data;

   draw_data = malloc(sizeof(*draw_data));
   EINA_SAFETY_ON_NULL_RETURN(draw_data);

   draw_data->ed = ed;
   draw_data->hflip = hflip;
   draw_data->colorize = colorize;

   elm_bitmap_abs_draw(ed->bitmap, draw_data, img, img_w, img_h, at_x, at_y);
   free(draw_data); /* XXX */
}

static void
_bitmap_init(Editor *restrict ed)
{
   int i, j, tile;
   const int init_tiles[] = {
      0x0050, 0x0051, 0x0052
   };
   const int seed = EINA_C_ARRAY_LENGTH(init_tiles);

   for (j = 0; j < ed->map_h; j++)
     {
        for (i = 0; i < ed->map_w; i++)
          {
             tile = init_tiles[rand() % seed];
             bitmap_tile_set(ed, i, j, tile);
          }
     }
}


/*============================================================================*
 *                                   Events                                   *
 *============================================================================*/

static void
_hovered_cb(void        *data,
            Evas_Object *bmp  EINA_UNUSED,
            void        *info)
{
   Editor *ed = data;
   Elm_Bitmap_Event_Hovered *ev = info;
   Cell c;

   c = ed->cells[ev->cell_y][ev->cell_x];

   if (texture_rock_is(c.tile) ||
       texture_wall_is(c.tile) ||
       texture_tree_is(c.tile))
     {
        /* Handle only flying units: they are the only one
         * that can be placed there */
        if (!pud_unit_flying_is(ed->sel_unit))
          elm_bitmap_cursor_enabled_set(ed->bitmap, EINA_FALSE);
        else
          {
             /* Don't collide with another unit */
             if (c.unit_above != PUD_UNIT_NONE)
               elm_bitmap_cursor_enabled_set(ed->bitmap, EINA_FALSE);
             else
               elm_bitmap_cursor_enabled_set(ed->bitmap, EINA_TRUE);
          }
     }
   else /* Ground, water */
     {
        /* Flying units */
        if (pud_unit_flying_is(ed->sel_unit))
          {
             /* Don't collide with another unit */
             if (c.unit_above != PUD_UNIT_NONE)
               elm_bitmap_cursor_enabled_set(ed->bitmap, EINA_FALSE);
             else
               elm_bitmap_cursor_enabled_set(ed->bitmap, EINA_TRUE);
          }
        else /* marine,ground units */
          {
             if (texture_water_is(c.tile)) /* water */
               {
                  if (pud_unit_marine_is(ed->sel_unit))
                    {
                       /* Don't collide with another unit */
                       if (c.unit_below != PUD_UNIT_NONE)
                         elm_bitmap_cursor_enabled_set(ed->bitmap, EINA_FALSE);
                       else
                         elm_bitmap_cursor_enabled_set(ed->bitmap, EINA_TRUE);
                    }
                  else
                    elm_bitmap_cursor_enabled_set(ed->bitmap, EINA_FALSE);
               }
             else /* ground */
               {
                  if (pud_unit_marine_is(ed->sel_unit))
                    elm_bitmap_cursor_enabled_set(ed->bitmap, EINA_FALSE);
                  else
                    {
                       if (c.unit_below != PUD_UNIT_NONE)
                         elm_bitmap_cursor_enabled_set(ed->bitmap, EINA_FALSE);
                       else
                         elm_bitmap_cursor_enabled_set(ed->bitmap, EINA_TRUE);
                    }
               }
          }
     }
}

static void
_clicked_cb(void        *data,
            Evas_Object *bmp  EINA_UNUSED,
            void        *info)
{
   Editor *ed = data;
   Elm_Bitmap_Event_Clicked *ev = info;
   Sprite_Info orient;

   if (!elm_bitmap_cursor_enabled_get(ed->bitmap)) return;

   if (ed->sel_unit != PUD_UNIT_NONE)
     {
        if (pud_unit_start_location_is(ed->sel_unit))
          {
             const int lx = ed->start_locations[ed->sel_player].x;
             const int ly = ed->start_locations[ed->sel_player].y;

             /* Start location did exist: move it. Also refresh
              * the zone where it was to remove it. */
             /* FIXME See Cedric's message on E-phab */
             if (ed->start_locations[ed->sel_player].x != -1)
               {
                  ed->cells[ly][lx].unit_below = PUD_UNIT_NONE;
                  bitmap_refresh_zone(ed, lx - 1, ly - 1, 3, 3);
                  if (ed->units_count > 0) ed->units_count++;
               }

             ed->start_locations[ed->sel_player].x = ev->cell_x;
             ed->start_locations[ed->sel_player].y = ev->cell_y;
          }

        /* Draw the unit, and therefore lock the cursor. */
        orient = sprite_info_random_get();
        bitmap_sprite_draw(ed, ed->sel_unit, ed->sel_player, orient, ev->cell_x, ev->cell_y);
        ed->units_count++;
        elm_bitmap_cursor_enabled_set(ed->bitmap, EINA_FALSE);
     }
}


/*============================================================================*
 *                                 Public API                                 *
 *============================================================================*/

void
bitmap_refresh_zone(Editor *restrict ed,
                    int              x,
                    int              y,
                    int              w,
                    int              h)
{
   int i, j;
   Cell c;

   /* Bounds checking - needed */
   if (x < 0) x = 0;
   if (y < 0) y = 0;
   if (x + w >= ed->map_w) w = ed->map_w - x - 1;
   if (y + h >= ed->map_h) h = ed->map_h - y - 1;

   for (j = y; j < y + h; j++)
     {
        for (i = x; i < x + w; i++)
          {
             c = ed->cells[j][i];
             bitmap_tile_set(ed, i, j, c.tile);
             if (c.anchor_below)
               bitmap_sprite_draw(ed, c.unit_below, c.player_below, c.orient_below, i, j);
          }
     }

   /* FIXME This is pretty bad!! To avoid mixing sprites I do 2 separate passes.
    * FIXME There is certainly much much better, I'll do it some day. */
   for (j = y; j < y + h; j++)
     {
        for (i = x; i < x + w; i++)
          {
             if (c.anchor_above)
               bitmap_sprite_draw(ed, c.unit_above, c.player_above, c.orient_above, i, j);
          }
     }
}

void
bitmap_sprite_draw(Editor *restrict ed,
                   Pud_Unit         unit,
                   Pud_Player       color,
                   unsigned int     orient,
                   int              x,
                   int              y)
{
   unsigned char *sprite;
   int w, h, cw, ch, cx, cy, at_x, at_y;
   int i, j;
   Eina_Bool flip;

   /* Don't draw */
   if (unit == PUD_UNIT_NONE) return;

   sprite = sprite_get(ed, unit, orient, NULL, NULL, &w, &h, &flip);
   EINA_SAFETY_ON_NULL_RETURN(sprite);

   eo_do(
      ed->bitmap,
      elm_obj_bitmap_cursor_size_get(&cw, &ch),
      elm_obj_bitmap_cursor_pos_get(&cx, &cy)
   );

   at_x = (cx * TEXTURE_WIDTH) + ((cw * TEXTURE_WIDTH) - w) / 2;
   at_y = (cy * TEXTURE_HEIGHT) + ((ch * TEXTURE_HEIGHT) - h) / 2;

   _draw(ed, sprite, at_x, at_y, w, h, flip, color);

   /* Bitfields, cannot take addresses to shorten that */
   if (pud_unit_flying_is(unit))
     {
        for (j = y; j < y + ch; ++j)
          {
             for (i = x; i < x + cw; ++i)
               {
                  ed->cells[j][i].unit_above = unit;
                  ed->cells[j][i].orient_above = orient;
                  ed->cells[j][i].player_above = color;
                  ed->cells[j][i].anchor_above = 0;
               }
          }
        ed->cells[y][x].anchor_above = 1;
     }
   else
     {
        for (j = y; j < y + ch; ++j)
          {
             for (i = x; i < x + cw; ++i)
               {
                  ed->cells[j][i].unit_below = unit;
                  ed->cells[j][i].orient_below = orient;
                  ed->cells[j][i].player_below = color;
                  ed->cells[j][i].anchor_below = 0;
               }
          }
        ed->cells[y][x].anchor_below = 1;
     }
}

Eina_Bool
bitmap_tile_set(Editor * restrict ed,
                int               x,
                int               y,
                unsigned int      key)
{
   unsigned char *tex;
   Eina_Bool missing;

   tex = texture_get(ed, key, &missing);
   /* If the texture could not be loaded because of an internal error,
    * return TRUE because we can do nothing about it.
    * If the texture was non-existant, let's try again: the tileset
    * is not helping us */
   if (!tex) return !missing;

   _draw(ed, tex, x * TEXTURE_WIDTH, y * TEXTURE_HEIGHT,
         TEXTURE_WIDTH, TEXTURE_HEIGHT, EINA_FALSE, -1);
   ed->cells[y][x].tile = key;
   return EINA_TRUE;
}

Eina_Bool
bitmap_add(Editor *ed)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(ed, EINA_FALSE);

   Evas_Object *obj;

   obj = elm_bitmap_init_add(ed->win, 32, 32, ed->map_w, ed->map_h);
   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, EINA_FALSE);

   eo_do(
      obj,
      evas_obj_size_hint_align_set(0.0, 0.0),
      evas_obj_size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND),
      elm_obj_bitmap_resizable_set(EINA_FALSE),
      elm_obj_bitmap_cursor_visibility_set(EINA_TRUE),
      elm_obj_bitmap_draw_func_set(_bitmap_draw_func)
   );
   evas_object_smart_callback_add(obj, "clicked", _clicked_cb, ed);
   evas_object_smart_callback_add(obj, "hovered", _hovered_cb, ed);

   ed->bitmap = obj;
   ed->cells = cell_matrix_new(ed);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ed->cells, EINA_FALSE);

   _bitmap_init(ed);

   return EINA_TRUE;
}

