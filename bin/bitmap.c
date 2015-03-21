#include "war2edit.h"

static const unsigned char _1x1_png[] =
{
   0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
   0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01,
   0x08, 0x02, 0x00, 0x00, 0x00, 0x90, 0x77, 0x53, 0xde, 0x00, 0x00, 0x00,
   0x0f, 0x49, 0x44, 0x41, 0x54, 0x08, 0x1d, 0x01, 0x04, 0x00, 0xfb, 0xff,
   0x00, 0xff, 0xff, 0xff, 0x05, 0xfe, 0x02, 0xfe, 0x03, 0x7d, 0x19, 0xc6,
   0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};


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

/*============================================================================*
 *                                 Public API                                 *
 *============================================================================*/

Eina_Bool
bitmap_add(Editor *ed)
{
   const int width = ed->map_w * 32;
   const int height = ed->map_h * 32;
#if 0
   const int min = texture_dictionary_min(ed);
   const int max = texture_dictionary_max(ed);
   Evas_Object *bmp;
   Evas_Object *img;
   void *data;
   int x, y;

   img = elm_image_add(ed->win);
   EINA_SAFETY_ON_NULL_RETURN_VAL(img, EINA_FALSE);
   eo_do(
      img,
      evas_obj_size_hint_align_set(EVAS_HINT_FILL, EVAS_HINT_FILL),
      evas_obj_size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND),
      elm_obj_image_aspect_fixed_set(EINA_TRUE),
      evas_obj_size_hint_min_set(width, height),
      evas_obj_size_hint_max_set(width, height),
      elm_obj_image_memfile_set(_1x1_png, sizeof(_1x1_png), "png", NULL)
   );
   ed->bitmap = img;

   bmp = elm_image_object_get(img);
   EINA_SAFETY_ON_NULL_RETURN_VAL(bmp, EINA_FALSE);

   ed->pixels = evas_object_image_data_get(bmp, EINA_TRUE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ed->pixels, EINA_FALSE);

//   memset(ed->pixels, 0, 1);




   ed->cells = _grid_cells_new(ed);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ed->cells, EINA_FALSE);

   for (y = 0; y < ed->map_h; ++y)
     {
        for (x = 0; x < ed->map_w; ++x)
          {
         //    _bitmap_tile_set(ed, x, y, (rand() % (max - min + 1)) + min);
          }
     }

#else

   Evas_Object *img;
   Evas *e;

   e = evas_object_evas_get(ed->win);
   img = evas_object_image_add(e);
   eo_do(
      img,
      evas_obj_size_hint_align_set(EVAS_HINT_FILL, EVAS_HINT_FILL),
      evas_obj_size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND),
      evas_obj_size_hint_min_set(width, height),
      evas_obj_size_hint_max_set(width, height)
   );
//   evas_object_image_memfile_set(img, (void *)_1x1_png, sizeof(_1x1_png), "png", NULL);
   evas_object_image_file_set(img, "/Users/Jean/Desktop/efl.png", NULL);
   ed->bitmap = img;

#endif


   return EINA_TRUE;
}


