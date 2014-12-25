#include "pudviewer.h"


static void
_usage(FILE *stream)
{
   fprintf(stream,
           "pudviewer %.1f - Warcraft II PUD viewer\n"
           "\n"
           "Usage:\n"
           "    pudviewer [options] <file.pud>\n"
           "\n"
           "Options:\n"
           "    -v | --verbose     Activate verbose mode\n"
           "    -h | --help        Shows this message\n"
           "\n",
           PUDVIEWER_VERSION);
}

int
main(int    argc,
     char **argv)
{
   int chk = 0;
   const char *file;
   Pud *pud;

   /* Getopt */
   if (argc >= 2)
     {
         
     }
   else
     {
        _usage(stderr);
        return 1;
     }

   file = argv[1];

   /* Open file */
   pud = pud_new(file);
   if (pud == NULL) DIE_GOTO(end, "Aborting pudviewer...");

   pud_verbose_set(pud, 1);
   pud_parse(pud);
   pud_minimap_to_ppm(pud, "minimap.ppm", 1.0f);

   pud_free(pud);

end:
   return chk;
}

