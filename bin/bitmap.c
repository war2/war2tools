#include "war2edit.h"

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

   _coords_to_grid(ed, ev->cur.canvas, &x, &y);
   cursor_pos_set(ed, x, y);
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

   if (!ed->cursor.enabled) return;

   _coords_to_grid(ed, ev->canvas, &x, &y);

   if (ed->sel_unit != EDITOR_NO_UNIT_SELECTED)
     bitmap_sprite_draw(ed, ed->sel_unit, ed->sel_player, x, y);
}


/*============================================================================*
 *                                 Public API                                 *
 *============================================================================*/

void
bitmap_sprite_draw(Editor *restrict ed,
                   Pud_Unit         unit,
                   Pud_Player       color,
                   int              x,
                   int              y)
{
   unsigned char *sprite;
   Sprite_Info info;
   int w, h, cw, ch, cx, cy, at_x, at_y;
   Eina_Bool flip;

   info = sprite_info_random_get();
   sprite = sprite_get(ed, unit, info, NULL, NULL, &w, &h, &flip);
   EINA_SAFETY_ON_NULL_RETURN(sprite);

   cursor_size_get(ed, &cw, &ch);
   cursor_pos_get(ed, &cx, &cy);

   at_x = (cx * TEXTURE_WIDTH) + ((cw * TEXTURE_WIDTH) - w) / 2;
   at_y = (cy * TEXTURE_HEIGHT) + ((ch * TEXTURE_HEIGHT) - h) / 2;

   /* Because the axis is inverted */
   _bitmap_image_push(ed, sprite, at_x, at_y, w, h, flip, color);

   /* FIXME: for all cells covered by the unit */
   ed->cells[y][x].unit = unit;
   ed->cells[y][x].orient = info;
   ed->cells[y][x].player = color;
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

   ed->cells = _grid_cells_new(ed);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ed->cells, EINA_FALSE);

   evas_object_event_callback_add(obj, EVAS_CALLBACK_MOUSE_MOVE, _mouse_move_cb, ed);
   evas_object_event_callback_add(obj, EVAS_CALLBACK_MOUSE_DOWN, _mouse_down_cb, ed);

   _bitmap_init(ed);

   return EINA_TRUE;
}

