#include "war2.h"

Pud_Bool
war2_ppm_write(const char          *file,
               int                  w,
               int                  h,
               const unsigned char *data)
{
   const int size = w * h * 4;
   FILE *f;
   int i;

   f = fopen(file, "w+");
   if (f == NULL) return PUD_FALSE;

   fprintf(f, "P3\n%i %i\n255\n", w, h);
   for (i = 0; i < size; i += 4)
     fprintf(f, "%i %i %i\n", data[i], data[i + 1], data[i + 2]);

   fclose(f);

   return PUD_TRUE;
}
