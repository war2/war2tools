#include "ppm.h"
#include "../pudviewer.h"

static inline int
_str2int(const char *str)
{
   return strtol(str, NULL, 10);
}

int
main(int    argc,
     char **argv)
{
   Pud *pud;
   Col *ppm;
   int w, h, i;
   uint16_t word;
   int x, y;

   if (argc != 5)
     {
        fprintf(stderr, "*** Usage: <file.pud> <file.ppm> <X> <Y>\n");
        return 1;
     }

   /* Parse PUD */
   pud = pud_new(argv[1]);
   if (pud == NULL)
     {
        fprintf(stderr, "*** pud_new() failed\n");
        return 2;
     }
   pud_parse(pud);

   /* Parse PPM */
   ppm = ppm_parse(argv[2], &w, &h);
   if (!ppm)
     {
        fprintf(stderr, "*** ppm_parse() failed\n");
        pud_free(pud);
        return 2;
     }

   /* Get tile */
   x = _str2int(argv[3]);
   y = _str2int(argv[4]);
   word = pud_tile_at(pud, x, y);
   if (word == 0)
     {
        fprintf(stderr, "*** pud_tile_at() failed\n");
        pud_free(pud);
        free(ppm);
        return 3;
     }

   i = y * w + x;
   printf("Tile [0x%04x] => { %02x, %02x, %02x }\n",
          ppm[i].r, ppm[i].g, ppm[i].b);


   pud_free(pud);
   free(ppm);

   return 0;
}

