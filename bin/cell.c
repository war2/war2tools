#include "war2edit.h"

Cell **
cell_matrix_new(Editor *ed)
{
   Cell **ptr;
   int i, j;

   /* Iliffe vector allocation */

   ptr = malloc(ed->map_h * sizeof(*ptr));
   EINA_SAFETY_ON_NULL_RETURN_VAL(ptr, NULL);

   ptr[0] = calloc(ed->map_w * ed->map_h, sizeof(**ptr));
   EINA_SAFETY_ON_NULL_GOTO(ptr[0], fail);

   for (i = 1; i < ed->map_h; ++i)
     ptr[i] = ptr[i - 1] + ed->map_w;

   for (i = 0; i < ed->map_h; i++)
     for (j = 0; j < ed->map_w; j++)
       ptr[i][j].unit = PUD_UNIT_NONE;
   /* Other fields are set to 0 */

   return ptr;

fail:
   free(ptr);
   return NULL;
}

void
cell_matrix_free(Cell **cells)
{
   if (cells)
     {
        free(cells[0]);
        free(cells);
     }
}

