#include "pudviewer.h"
#include <getopt.h>

static struct option _options[] =
{
     {"verbose",  no_argument,    0, 'v'},
     {"help",     no_argument,    0, 'h'}
};

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
   int chk = 0, c, opt_idx = 0;
   const char *file;
   Pud *pud;
   int verbose = 0;

   /* Getopt */
   while (1)
     {
        c = getopt_long(argc, argv, "hv", _options, &opt_idx);
        if (c == -1) break;

        switch (c)
          {
           case 'v':
              verbose++;
              break;

           case 'h':
              _usage(stdout);
              return 0;
          }
     }
   if (argc - optind != 1)
     {
        ERR("pudviewer requires one argument.");
        ERR("Relaunch with option --help for hints");
        return 1;
     }

   file = argv[optind];

   /* Open file */
   pud = pud_new(file);
   if (pud == NULL) DIE_GOTO(end, "Aborting pudviewer...");

   pud_verbose_set(pud, 1);
   pud_parse(pud);
   pud_minimap_to_ppm(pud, "minimap.ppm");
   pud_minimap_to_jpeg(pud, "minimap.jpeg");

   pud_free(pud);

end:
   return chk;
}

