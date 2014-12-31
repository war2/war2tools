#include "pudutils.h"

static const struct option _options[] =
{
     {"output",   required_argument,    0, 'o'},
     {"tile-at",  required_argument,    0, 't'},
     {"ppm",      no_argument,          0, 'p'},
     {"jpeg",     no_argument,          0, 'j'},
     {"png",      no_argument,          0, 'g'},
     {"print",    no_argument,          0, 'P'},
     {"sections", no_argument,          0, 's'},
     {"war",      no_argument,          0, 'W'},
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
           "    -W | --war           The file to parse is a .WAR file instead of a .PUD\n"
           "    -P | --print         Prints the data in stdout\n"
           "    -o | --output <file> When -p or -j is present outputs the file with the provided filename\n"
           "    -p | --ppm           Outputs the minimap as a ppm file. If --out is not specified,\n"
           "                         the output's filename will the the input file plus \".ppm\"\n"
           "    -j | --jpeg          Outputs the minimap as a jpeg file. If --out is not specified,\n"
           "                         the output's filename will the the input file plus \".jpeg\"\n"
           "    -g | --png           Outputs the minimap as a png file. If --out is not specified,\n"
           "                         the output's filename will the the input file plus \".png\"\n"
           "    -t | --tile-at <x,y> Gets the tile ID at x,y\n"
           "    -s | --sections      Gets sections in the file.\n"
           "    -v | --verbose       Activate verbose mode. Cumulate flags increase verbosity level.\n"
           "    -h | --help          Shows this message\n"
           "\n",
           VERSION);
}

#define ABORT(errcode_, msg, ...) \
   do { ret_status = errcode_;  ERR(msg, ## __VA_ARGS__); goto end; } while (0)

#define ZERO(struct_) memset(&struct_, 0, sizeof(struct_))

int
main(int    argc,
     char **argv)
{
   int chk, c, opt_idx = 0, ret_status = EXIT_SUCCESS;
   const char *file;
   Pud *pud = NULL;
   War2_Data *w2 = NULL;
   int verbose = 0;
   uint16_t w;
   bool war2 = false;
   int i;

   struct  {
      int          x;
      int          y;
      unsigned int enabled : 1;
   } tile_at;

   struct {
      char         *file;
      unsigned int  ppm     : 1;
      unsigned int  jpeg    : 1;
      unsigned int  png     : 1;
      unsigned int  enabled : 1;
   } out;

   struct {
      unsigned int enabled : 1;
   } print;

   struct {
      unsigned int enabled : 1;
   } sections;


   /* Reset all opts */
   ZERO(tile_at);
   ZERO(out);
   ZERO(print);
   ZERO(sections);

   /* Getopt */
   while (1)
     {
        c = getopt_long(argc, argv, "o:pjshgWPvt:", _options, &opt_idx);
        if (c == -1) break;

        switch (c)
          {
           case 'v':
              verbose++;
              break;

           case 'h':
              _usage(stdout);
              return 0;

           case 's':
              sections.enabled = 1;
              break;

           case 'W':
              war2 = true;
              break;

           case 't':
              tile_at.enabled = 1;
              sscanf(optarg, "%i,%i", &tile_at.x, &tile_at.y);
              break;

           case 'p':
              out.ppm = 1;
              out.enabled = 1;
              break;

           case 'g':
              out.png = 1;
              out.enabled = 1;
              break;

           case 'j':
              out.jpeg = 1;
              out.enabled = 1;
              break;

           case 'o':
              out.enabled = 1;
              out.file = strdup(optarg);
              if (!out.file) ABORT(2, "Failed to strdup [%s]", optarg);
              break;

           case 'P':
              print.enabled = 1;
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
   if (file == NULL) ABORT(1, "NULL input file");

   if (war2 == true)
     {
        if (tile_at.enabled ||
            out.enabled     ||
            print.enabled)
          ABORT(1, "Invalid option when --war,-W is specified");

        w2 = war2_open(file, verbose);
        if (w2 == NULL) ABORT(3, "Failed to create War2_Data from [%s]", file);

        /* -s,--sections */
        if (sections.enabled)
          fprintf(stdout, "%i\n", w2->entries_count);
     }
   else
     {
        /* Open file */
        pud = pud_open(file, PUD_OPEN_MODE_R);
        if (pud == NULL) ABORT(3, "Failed to create pud from [%s]", file);

        /* Set verbosity level */
        pud_verbose_set(pud, verbose);

        /* Parse pud */
        if (!pud_parse(pud))
          ABORT(5, "Parsing of [%s] failed", file);

        /* --tile-at */
        if (tile_at.enabled)
          {
             w = pud_tile_at(pud, tile_at.x, tile_at.y);
             if (w == 0) ABORT(3, "Failed to parse tile");
             fprintf(stdout, "0x%04x\n", w);
          }

        /* --output,--ppm,--jpeg */
        if (out.enabled)
          {
             if (out.jpeg + out.ppm + out.png != 1)
               ABORT(1, "--jpeg,--ppm,--png cannot be combined together.");

             if (!out.file)
               {
                  char buf[4096];
                  int len;
                  const char *ext;

                  if (out.ppm)       ext = "ppm";
                  else if (out.jpeg) ext = "jpeg";
                  else if (out.png)  ext = "png";
                  else ABORT(1, "Output is required but no format is specified");

                  len = snprintf(buf, sizeof(buf), "%s.%s", file, ext);
                  out.file = strndup(buf, len);
                  if (!out.file) ABORT(2, "Failed to strdup [%s]", buf);
               }

             if (out.ppm)
               {
                  if (!pud_minimap_to_ppm(pud, out.file))
                    {
                       free(out.file);
                       ABORT(4, "Failed to output [%s] to [%s]", file, out.file);
                    }
               }
             else if (out.jpeg)
               {
                  if (!pud_minimap_to_jpeg(pud, out.file))
                    {
                       free(out.file);
                       ABORT(4, "Failed to output [%s] to [%s]", file, out.file);
                    }
               }
             else if (out.png)
               {
                  if (!pud_minimap_to_png(pud, out.file))
                    {
                       free(out.file);
                       ABORT(4, "Failed to output [%s] to [%s]", file, out.file);
                    }
               }
             else
               ABORT(1, "Output is required no format is specified");

             free(out.file);
          }

        /* -P,--print */
        if (print.enabled)
          pud_print(pud, stdout);

        /* -s,--sections */
        if (sections.enabled)
          {
             for (i = 0; i < 20; i++)
               {
                  if ((pud->sections & (1 << i)))
                    fprintf(stdout, "%s\n", pud_section_at_index(i));
               }
          }
     }

end:
   pud_close(pud);
   war2_close(w2);
   return ret_status;
}

