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


/*============================================================================*
 *                               Event Callbacks                              *
 *============================================================================*/

//#define IS_KEY(keystr) strncmp(ev->key, keystr, sizeof(keystr) - 1) == 0
//static void
//_key_down_cb(void        *data,
//             Evas        *evas  EINA_UNUSED,
//             Evas_Object *bm,
//             void        *event)
//{
//   Evas_Event_Key_Down *ev = event;
//   Editor *ed = data;
//   Eina_Bool refresh = EINA_FALSE;
//
//   if (evas_key_modifier_is_set(ev->modifiers, "Alt"))
//     return;
//
//   if (evas_key_modifier_is_set(ev->modifiers, "Control"))
//     {
//        if (IS_KEY("plus"))
//          {
//   //          ed->gl.zoom += ed->gl.zoom_step;
//             refresh = EINA_TRUE;
//          }
//        else if (IS_KEY("minus"))
//          {
//       //      if (ed->gl.zoom > ed->gl.zoom_step)
//     //          ed->gl.zoom -= ed->gl.zoom_step;
//             refresh = EINA_TRUE;
//          }
//     }
//   else
//     {
//        if (IS_KEY("Up"))
//          {
////             ed->gl.y += 0.01f;
//             refresh = EINA_TRUE;
//          }
//        else if (IS_KEY("Down"))
//          {
//  //           ed->gl.y -= 0.01f;
//             refresh = EINA_TRUE;
//          }
//        else if (IS_KEY("Left"))
//          {
//    //         ed->gl.x -= 0.01f;
//             refresh = EINA_TRUE;
//          }
//        else if (IS_KEY("Right"))
//          {
//      //       ed->gl.x += 0.01f;
//             refresh = EINA_TRUE;
//          }
//     }
//
////   if (refresh)
////     elm_glview_changed_set(glv);
//}
#undef IS_KEY

#if 0
static void
_bitmap_image_push(unsigned char * restrict bmp,
                   unsigned char * restrict img,
                   int                      at_x,
                   int                      at_y,
                   int                      img_w,
                   int                      img_h,
                   int                      bmp_w,
                   int                      bmp_h)
{
   int y_img, y_bmp;

   for (y_img = 0, y_bmp = at_y; (y_img < img_h) || (y_bmp < bmp_h); ++y_img, ++y_bmp)
     memcpy(&(bmp[(y_bmp * bmp_w) + at_x]), &(img[y_img * img_w]), img_w);
}

static inline unsigned char *
_bitmap_data_get(Editor *ed)
{
   return evas_object_image_data_get(ed->bitmap, EINA_TRUE);
}

static inline void
_bitmap_image_update(Editor *ed,
                     int     x,
                     int     y,
                     int     w,
                     int     h)
{
   evas_object_image_data_update_add(ed->bitmap, x, y, w, h);
}

static inline void
_bitmap_tile_update(Editor *ed,
                    int     x,
                    int     y)
{
   _bitmap_image_update(ed, x, y, 32, 32);
}

static inline void
_bitmap_tile_set(Editor       *ed,
                 int           x,
                 int           y,
                 unsigned int  key)
{
   unsigned char *tex, *bmp;
   const int size = ed->map_w * 32;

   tex = texture_get(ed, key);
   EINA_SAFETY_ON_NULL_RETURN(tex);

   bmp = _bitmap_data_get(ed);
   EINA_SAFETY_ON_NULL_RETURN(bmp);

   _bitmap_image_push(bmp, tex, x, y, 32, 32, size, size);
   _bitmap_tile_update(ed, x, y);

   ed->cells[y][x].tile = key;
}
#endif

/*============================================================================*
 *                                 Public API                                 *
 *============================================================================*/

Eina_Bool
bitmap_add(Editor *ed)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(ed, EINA_FALSE);

   const int width = ed->map_w * 32;
   const int height = ed->map_h * 32;
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
      evas_obj_image_size_set(width, height),
      evas_obj_size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND),
      evas_obj_size_hint_align_set(0.0, 0.0),
      evas_obj_size_hint_min_set(width, height),
      evas_obj_size_hint_max_set(width, height),
      evas_obj_image_data_set(mem)
   );

   memset(mem, 255, size);

   ed->bitmap = obj;
   ed->pixels = mem;

   ed->cells = _grid_cells_new(ed);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ed->cells, EINA_FALSE);

   return EINA_TRUE;
}

