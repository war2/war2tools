#include "war2edit.h"

/*============================================================================*
 *                                 Private API                                *
 *============================================================================*/

static void
_bitmap_image_push(Editor        *          ed,
                   unsigned char * restrict img,
                   int                      at_x,
                   int                      at_y,
                   int                      img_w,
                   int                      img_h,
                   Eina_Bool                hflip,
                   int                      colorize)
{
   const int bmp_w = ed->bitmap_w * 4;
   const int bmp_h = ed->bitmap_h;
   const int x = at_x;
   const int w = img_w;
   int img_y, bmp_y, img_x, bmp_x, k;
   unsigned char *restrict bmp = ed->pixels;
   unsigned char bgr[4];

   int bmp_x_start;
   int bmp_x_step;

   img_w *= 4;
   at_x *= 4;

   /* Always used when there is full alpha */
   for (img_y = 0, bmp_y = at_y;
        (img_y < img_h) && (bmp_y < bmp_h);
        ++img_y, ++bmp_y)
     {
        /* Calculate the horizontal mirroring  */
        if (hflip)
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
             memcpy(&(bgr[0]), &(img[k]), 4);
             if (colorize != -1)
               {
                  war2_sprites_color_convert(colorize,
                                             bgr[2], bgr[1], bgr[0],
                                             &(bgr[2]), &(bgr[1]), &(bgr[0]));
               }
             if (bgr[3] != 0)
               {
                  memcpy(&(bmp[bmp_x + bmp_y * bmp_w]), &(bgr[0]), 4);
               }
          }
     }

   evas_object_image_data_update_add(ed->bitmap, x, at_y, w, img_h);
}

static void
_bitmap_init(Editor *restrict ed)
{
   int i, j, tile;

   for (j = 0; j < ed->map_h; j++)
     {
        for (i = 0; i < ed->map_w; i++)
          {
             // FIXME This is pretty bad
             // FIXME Study borders between tiles for a better algorithm
             tile = texture_dictionary_entry_random_get(&ed->tex_dict.constr);
             bitmap_tile_set(ed, i, j, tile);
          }
     }
}

static void
_coords_to_grid(Editor *restrict  ed,
                Evas_Coord_Point  cur,
                int              *x,
                int              *y)
{
   Evas_Point scr_view;
   int cell_x, cell_y;

   /* Which cell in the grid are we pointing at? */
   elm_scroller_region_get(ed->scroller, &scr_view.x, &scr_view.y, NULL, NULL);
   cell_x = (cur.x + scr_view.x - ed->bitmap_origin.x) / TEXTURE_WIDTH;
   cell_y = (cur.y + scr_view.y - ed->bitmap_origin.y) / TEXTURE_HEIGHT;

   *x = cell_x;
   *y = cell_y;
}


/*============================================================================*
 *                                   Events                                   *
 *============================================================================*/

static void
_mouse_move_cb(void        *data,
               Evas        *evas EINA_UNUSED,
               Evas_Object *bmp  EINA_UNUSED,
               void        *info)
{
   Editor *ed = data;
   Evas_Event_Mouse_Move *ev = info;
   int x, y;
   Cell c;

   _coords_to_grid(ed, ev->cur.canvas, &x, &y);
   cursor_pos_set(ed, x, y);

   c = ed->cells[y][x];

   if (texture_rock_is(&ed->tex_dict, c.tile) ||
       texture_wall_is(&ed->tex_dict, c.tile) ||
       texture_tree_is(&ed->tex_dict, c.tile))
     {
        /* Handle only flying units: they are the only one
         * that can be placed there */
        if (!pud_unit_flying_is(ed->sel_unit))
          cursor_disable(ed);
        else
          {
             /* Don't collide with another unit */
             if (c.unit_above != PUD_UNIT_NONE)
               cursor_disable(ed);
             else
               cursor_enable(ed);
          }
     }
   else /* Ground, water */
     {
        /* Flying units */
        if (pud_unit_flying_is(ed->sel_unit))
          {
             /* Don't collide with another unit */
             if (c.unit_above != PUD_UNIT_NONE)
               cursor_disable(ed);
             else
               cursor_enable(ed);
          }
        else /* marine,ground units */
          {
             if (texture_water_is(&ed->tex_dict, c.tile)) /* water */
               {
                  if (pud_unit_marine_is(ed->sel_unit))
                    {
                       /* Don't collide with another unit */
                       if (c.unit_below != PUD_UNIT_NONE)
                         cursor_disable(ed);
                       else
                         cursor_enable(ed);
                    }
                  else
                    cursor_disable(ed);
               }
             else /* ground */
               {
                  if (pud_unit_marine_is(ed->sel_unit))
                    cursor_disable(ed);
                  else
                    {
                       if (c.unit_below != PUD_UNIT_NONE)
                         cursor_disable(ed);
                       else
                         cursor_enable(ed);
                    }
               }
          }
     }
}

static void
_mouse_down_cb(void        *data,
               Evas        *evas EINA_UNUSED,
               Evas_Object *bmp  EINA_UNUSED,
               void        *info)
{
   Editor *ed = data;
   Evas_Event_Mouse_Down *ev = info;
   int x, y;
   Sprite_Info orient;

   if (!ed->cursor.enabled) return;

   _coords_to_grid(ed, ev->canvas, &x, &y);

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
               bitmap_refresh_zone(ed, lx - 1, ly - 1, 3, 3);

             ed->start_locations[ed->sel_player].x = x;
             ed->start_locations[ed->sel_player].y = y;
          }

        /* Draw the unit, and therefore lock the cursor. */
        orient = sprite_info_random_get();
        bitmap_sprite_draw(ed, ed->sel_unit, ed->sel_player, orient, x, y);
        cursor_disable(ed);
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
   Eina_Bool flip;

   /* Don't draw */
   if (unit == PUD_UNIT_NONE) return;

   sprite = sprite_get(ed, unit, orient, NULL, NULL, &w, &h, &flip);
   EINA_SAFETY_ON_NULL_RETURN(sprite);

   cursor_size_get(ed, &cw, &ch);
   cursor_pos_get(ed, &cx, &cy);

   at_x = (cx * TEXTURE_WIDTH) + ((cw * TEXTURE_WIDTH) - w) / 2;
   at_y = (cy * TEXTURE_HEIGHT) + ((ch * TEXTURE_HEIGHT) - h) / 2;

   /* Because the axis is inverted */
   _bitmap_image_push(ed, sprite, at_x, at_y, w, h, flip, color);

   /* FIXME: for all cells covered by the unit */
   if (pud_unit_flying_is(unit))
     {
        ed->cells[y][x].unit_above = unit;
        ed->cells[y][x].orient_above = orient;
        ed->cells[y][x].player_above = color;
     }
   else
     {
        ed->cells[y][x].unit_below = unit;
        ed->cells[y][x].orient_below = orient;
        ed->cells[y][x].player_below = color;
     }
}

void
bitmap_tile_set(Editor * restrict ed,
                int               x,
                int               y,
                unsigned int      key)
{
   unsigned char *tex;

   tex = texture_get(ed, key);
   EINA_SAFETY_ON_NULL_RETURN(tex);

   _bitmap_image_push(ed, tex, x * TEXTURE_WIDTH, y * TEXTURE_HEIGHT,
                      TEXTURE_WIDTH, TEXTURE_HEIGHT, EINA_FALSE, -1);
   ed->cells[y][x].tile = key;
}

Eina_Bool
bitmap_add(Editor *ed)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(ed, EINA_FALSE);

   const int width = ed->map_w * TEXTURE_WIDTH;
   const int height = ed->map_h * TEXTURE_HEIGHT;
   const int size = width * height * 4 * sizeof(unsigned char);
   Evas *e;
   Evas_Object *obj;
   unsigned char *mem;

   e = evas_object_evas_get(ed->win);
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, EINA_FALSE);

   obj = evas_object_image_filled_add(e);
   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, EINA_FALSE);

   mem = calloc(size, sizeof(unsigned char));
   EINA_SAFETY_ON_NULL_RETURN_VAL(mem, EINA_FALSE);

   eo_do(
      obj,
      evas_obj_image_colorspace_set(EVAS_COLORSPACE_ARGB8888),
      evas_obj_image_size_set(width, height),
      evas_obj_size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND),
      evas_obj_size_hint_align_set(0.0, 0.0),
      evas_obj_size_hint_min_set(width, height),
      evas_obj_size_hint_max_set(width, height),
      evas_obj_image_data_set(mem)
   );

   ed->bitmap = obj;
   ed->pixels = mem;
   ed->bitmap_w = width;
   ed->bitmap_h = height;

   ed->cells = cell_matrix_new(ed);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ed->cells, EINA_FALSE);

   evas_object_event_callback_add(obj, EVAS_CALLBACK_MOUSE_MOVE, _mouse_move_cb, ed);
   evas_object_event_callback_add(obj, EVAS_CALLBACK_MOUSE_DOWN, _mouse_down_cb, ed);

   _bitmap_init(ed);

   return EINA_TRUE;
}

