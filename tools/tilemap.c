#include <pud.h>
#include "ppm.h"
#include "../include/debug.h"

static void
_usage(void)
{
   fprintf(stderr, "*** Usage: tilemap <file.ppm> <map.txt>\n");
   fprintf(stderr,
           "*** Map file format (x,y,tile):\n"
           "%%i %%i 0x%%04x\\n\n");
}

int
main(int    argc,
     char **argv)
{
   Col *ppm;
   Col c;
   int w, h;
   FILE *f;
   int x, y, tile;

   /* Getopt */
   if (argc != 3)
     {
        _usage();
        return 1;
     }

   ppm = ppm_parse(argv[1], &w, &h);
   if (!ppm) DIE_RETURN(1, "Failed to parse [%s]", argv[1]);

   f = fopen(argv[2], "r");
   if (!f) DIE_RETURN(1, "Failed to open [%s]", argv[2]);

   while ((!feof(f)) && (!ferror(f)))
     {
        fscanf(f, "%i %i %x\n", &x, &y, &tile);
        c = ppm[x + (y * w)];

        /* Ignore black (nonexistant tile) */
        if ((c.r == 0) && (c.g == 0) && (c.b == 0))
          continue;

        printf("case 0x%04x: return color_make(0x%02x, 0x%02x, 0x%02x);\n",
               tile, c.r, c.g, c.b);
     }


   if (ferror(f))
     {
        fclose(f);
        free(ppm);
        DIE_RETURN(2, "Ferror()");
     }


   fclose(f);
   free(ppm);

   return 0;
}

