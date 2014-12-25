#include "pudviewer.h"
#include <getopt.h>

static const struct option _options[] =
{
     {"tile-at",  required_argument,    0, 't'},
     {"verbose",  no_argument,          0, 'v'},
     {"help",     no_argument,          0, 'h'}
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
           "    -t | --tile-at <x,y> Gets the tile ID at x,y\n"
           "    -v | --verbose       Activate verbose mode\n"
           "    -h | --help          Shows this message\n"
           "\n",
           PUDVIEWER_VERSION);
}

#define ABORT(errcode_) \
   do { chk = errcode_; goto abort; } while (0)

int
main(int    argc,
     char **argv)
{
   int chk = 0, c, opt_idx = 0;
   const char *file;
   Pud *pud;
   int verbose = 0;
   uint16_t w;

   struct  {
      int          x;
      int          y;
      unsigned int enabled : 1;
   } tile_at;

   /* Reset all opts */
   tile_at.enabled = 0;

   /* Getopt */
   while (1)
     {
        c = getopt_long(argc, argv, "hvt:", _options, &opt_idx);
        if (c == -1) break;

        switch (c)
          {
           case 'v':
              verbose++;
              break;

           case 'h':
              _usage(stdout);
              return 0;

           case 't':
              tile_at.enabled = 1;
              sscanf(optarg, "%i,%i", &tile_at.x, &tile_at.y);
              break;
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

   pud_verbose_set(pud, verbose);
   pud_parse(pud);

   if (tile_at.enabled)
     {
        w = pud_tile_at(pud, tile_at.x, tile_at.y);
        if (w == 0) ABORT(2);
        fprintf(stdout, "0x%04x\n", w);
     }

   pud_minimap_to_ppm(pud, "minimap.ppm");
   pud_minimap_to_jpeg(pud, "minimap.jpeg");

abort:
   pud_free(pud);

end:
   return chk;
}

