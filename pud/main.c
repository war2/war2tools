/*
 * main.c
 * pud
 *
 * Copyright (c) 2014 - 2016 Jean Guyomarc'h
 */

#include "pudutils.h"

#ifndef HAVE_STRNDUP
static char *
strndup(const char *str, size_t size)
{
  char *const mem = malloc(size + 1);
  memcpy(mem, str, size);
  mem[size] = '\0';
  return mem;
}
#endif

static const struct option _options[] =
{
     {"output",   required_argument,    0, 'o'},
     {"tile-at",  required_argument,    0, 't'},
     {"sprite",   required_argument,    0, 'S'},
     {"ppm",      no_argument,          0, 'p'},
     {"jpeg",     no_argument,          0, 'j'},
     {"png",      no_argument,          0, 'g'},
     {"print",    no_argument,          0, 'P'},
     {"regm",     no_argument,          0, 'R'},
     {"sqm",      no_argument,          0, 'Q'},
     {"sections", no_argument,          0, 's'},
     {"war",      no_argument,          0, 'W'},
     {"verbose",  no_argument,          0, 'v'},
     {"help",     no_argument,          0, 'h'},
     {NULL,       0,                    0, '\0'}
};

static void
_usage(FILE *stream)
{
   fprintf(stream,
           PROGRAM" %.1f - Warcraft II PUD utilities\n"
           "\n"
           "Usage:\n"
           "    "PROGRAM" [options] <file.pud>\n"
           "\n"
           "Options:\n"
           "    -W | --war            The file to parse is a .WAR file instead of a .PUD\n"
           "    -P | --print          Prints the data in stdout\n"
           "    -o | --output <file>  When -p or -j is present outputs the file with the provided filename\n"
           "    -p | --ppm            Outputs the minimap as a ppm file. If --out is not specified,\n"
           "                          the output's filename will the the input file plus \".ppm\"\n"
           "    -j | --jpeg           Outputs the minimap as a jpeg file. If --out is not specified,\n"
           "                          the output's filename will the the input file plus \".jpeg\"\n"
           "    -g | --png            Outputs the minimap as a png file. If --out is not specified,\n"
           "                          the output's filename will the the input file plus \".png\"\n"
           "    -t | --tile-at <x,y>  Gets the tile ID at x,y\n"
           "    -R | --regm           Writes the action map\n"
           "    -Q | --sqm            Writes the movement map\n"
           "    -s | --sections       Gets sections in the PUD file.\n"
           "    -S | --sprite <entry> Extract the graphic entry specified. Only when -W is enabled.\n"
           "                  <color> An output file (with -o) and type (-p,-j,-g) must be provided.\n"
           "                          Color must be a string (red, blue, ...). Arguments must be\n"
           "                          comma-separated\n"
           "\n"
           "    -v | --verbose        Activate verbose mode. Cumulate flags increase verbosity level.\n"
           "    -h | --help           Shows this message\n"
           "\n",
           VERSION);
}


static struct  {
   int          x;
   int          y;
   unsigned int enabled : 1;
} tile_at;

static struct {
   char         *file;
   unsigned int  ppm     : 1;
   unsigned int  jpeg    : 1;
   unsigned int  png     : 1;
   unsigned int  enabled : 1;
} out;

static struct {
   unsigned int enabled : 1;
} print;

static struct {
   unsigned int enabled : 1;
} sections;

static struct {
   unsigned int enabled : 1;
   unsigned int entry;
   Pud_Player   color;
   /*Pud_Era      era;*/
} sprite;

static struct {
   unsigned int enabled : 1;
} regm;

static struct {
   unsigned int enabled : 1;
} sqm;



#define ABORT(errcode_, msg, ...) \
   do { ret_status = errcode_;  ERR(msg, ## __VA_ARGS__); goto end; } while (0)

#define ZERO(struct_) memset(&struct_, 0, sizeof(struct_))

#define IS_STR(str_) !strncasecmp(str, str_, sizeof(str_) - 1)

#if 0
static Pud_Era
_str2era(const char *str)
{
   if (IS_STR("forest"))
     return PUD_ERA_FOREST;
   else if (IS_STR("winter"))
     return PUD_ERA_WINTER;
   else if (IS_STR("wasteland"))
     return PUD_ERA_WASTELAND;
   else if (IS_STR("swamp"))
     return PUD_ERA_SWAMP;
   else
     {
        fprintf(stderr, "*** Invalid ERA <%s>\n", str);
        exit(1);
     }
}
#endif

static Pud_Player
_str2color(const char *str)
{
   if (IS_STR("red"))
     return PUD_PLAYER_RED;
   else if (IS_STR("blue"))
     return PUD_PLAYER_BLUE;
   else if (IS_STR("green"))
     return PUD_PLAYER_GREEN;
   else if (IS_STR("violet"))
     return PUD_PLAYER_VIOLET;
   else if (IS_STR("orange"))
     return PUD_PLAYER_ORANGE;
   else if (IS_STR("black"))
     return PUD_PLAYER_BLACK;
   else if (IS_STR("white"))
     return PUD_PLAYER_WHITE;
   else if (IS_STR("yellow"))
     return PUD_PLAYER_YELLOW;
   else
     {
        fprintf(stderr, "*** Invalid COLOR <%s>\n", str);
        exit(1);
     }
}

static void
_war2_entry_cb(void                          *data,
               const Pud_Color               *img,
               int                            x,
               int                            y,
               unsigned int                   w,
               unsigned int                   h,
               const War2_Sprites_Descriptor *ud,
               uint16_t                       img_nb)
{
   char file[4096];
   Pud_Bool chk = PUD_FALSE;

   if (out.png)
     {
        snprintf(file, sizeof(file), "%s_%i.png", out.file, img_nb);
        chk = war2_png_write(file, w, h, (const unsigned char *)img);
     }
   else if (out.jpeg)
     {
        snprintf(file, sizeof(file), "%s_%i.jpg", out.file, img_nb);
        chk = war2_jpeg_write(file, w, h, (const unsigned char *)img);
     }
   else if (out.ppm)
     {
        snprintf(file, sizeof(file), "%s_%i.ppm", out.file, img_nb);
        chk = war2_ppm_write(file, w, h, (const unsigned char *)img);
     }
   if (!chk)
     {
        fprintf(stderr, "*** Failed to save to [%s]", file);
        exit(2);
     }
   printf("Saving sprite %i at \"%s\". X,Y,W,H = %i,%i,%i,%i\n",
          img_nb, file, x, y, w, h);

   (void) ud;
}


int
main(int    argc,
     char **argv)
{
   int c, opt_idx = 0, ret_status = EXIT_SUCCESS;
   const char *file;
   Pud *pud = NULL;
   War2_Data *w2 = NULL;
   War2_Sprites_Descriptor *ud;
   int verbose = 0;
   uint16_t w;
   Pud_Bool war2 = PUD_FALSE;
   int i;
   char *ptr = NULL;

   /* Getopt */
   while (1)
     {
        c = getopt_long(argc, argv, "o:pjsS:hgWPRQvt:", _options, &opt_idx);
        if (c == -1) break;

        switch (c)
          {
           case 'v':
              verbose++;
              break;

           case 'h':
              _usage(stdout);
              return 0;

           case 'S':
              sprite.enabled = 1;
              sprite.entry = strtol(optarg, &ptr, 10);
              sprite.color = _str2color(ptr + 1);
              break;

           case 'R':
              regm.enabled = 1;
              break;

           case 'Q':
              sqm.enabled = 1;
              break;

           case 's':
              sections.enabled = 1;
              break;

           case 'W':
              war2 = PUD_TRUE;
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

           default:
              return 1;
          }
     }

   if (argc - optind != 1)
     {
        ERR(PROGRAM" requires one argument.");
        ERR("Relaunch with option --help for hints");
        return 1;
     }

   file = argv[optind];
   if (file == NULL) ABORT(1, "NULL input file");

   if (war2 == PUD_TRUE)
     {
        if (tile_at.enabled ||
            print.enabled   ||
            regm.enabled    ||
            sqm.enabled     ||
            sections.enabled)
          ABORT(1, "Invalid option when --war,-W is specified");

        w2 = war2_open(file);
        if (w2 == NULL) ABORT(3, "Failed to create War2_Data from [%s]", file);
        war2_verbosity_set(w2, verbose);

        if (sprite.enabled)
          {
             if (!out.enabled)
               ABORT(1, "You must use -o with this option");
             if (out.jpeg + out.ppm + out.png != 1)
               ABORT(1, "You must use one of --jpeg,--ppm,--png.");

             war2_sprites_decode_entry(w2, sprite.color, sprite.entry, _war2_entry_cb, NULL);
          }
     }
   else
     {
        if (sprite.enabled)
          ABORT(1, "Invalid option when --war,-W is not specified");

        /* Open file */
        pud = pud_open(file, PUD_OPEN_MODE_R);
        if (pud == NULL) ABORT(3, "Failed to create pud from [%s]", file);

        /* --tile-at */
        if (tile_at.enabled)
          {
             const unsigned int idx = (tile_at.y * pud->map_w) + tile_at.x;
             w = pud_tile_get(pud, tile_at.x, tile_at.y);
             if (w == 0) ABORT(3, "Failed to parse tile");
             fprintf(stdout,
                     "ID.....: 0x%04x\n"
                     "REGM...: 0x%04x\n"
                     "SQM....: 0x%04x\n"
                     ,
                     w, pud->action_map[idx], pud->movement_map[idx]);
          }

        /* --output,--ppm,--jpeg,--png */
        if (out.enabled)
          {
             if (regm.enabled)
               ABORT(1, "--regm is not compatible with --output");
             if (sqm.enabled)
               ABORT(1, "--sqm is not compatible with --output");
             if (out.jpeg + out.ppm + out.png != 1)
               ABORT(1, "You must use one of --jpeg,--ppm,--png.");

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
                    ABORT(4, "Failed to output [%s] to [%s]", file, out.file);
               }
             else if (out.jpeg)
               {
                  if (!pud_minimap_to_jpeg(pud, out.file))
                    ABORT(4, "Failed to output [%s] to [%s]", file, out.file);
               }
             else if (out.png)
               {
                  if (!pud_minimap_to_png(pud, out.file))
                    ABORT(4, "Failed to output [%s] to [%s]", file, out.file);
               }
             else
               ABORT(1, "Output is required no format is specified");
          }
        if (regm.enabled)
          {
             for (i = 0; i < (int) (pud->map_w * pud->map_h); ++i)
               {
                  fprintf(stdout, "0x%04x ", pud->action_map[i]);
                  if (((i + 1) % pud->map_w) == 0)
                    fputc('\n', stdout);
               }
          }
        if (sqm.enabled)
          {
             for (i = 0; i < (int) (pud->map_w * pud->map_h); ++i)
               {
                  fprintf(stdout, "0x%04x ", pud->movement_map[i]);
                  if (((i + 1) % pud->map_w) == 0)
                    fputc('\n', stdout);
               }
          }

        /* -P,--print */
        if (print.enabled)
          pud_dump(pud, stdout);

        /* -s,--sections */
        if (sections.enabled)
          {
             for (i = 0; i < 20; i++)
               {
                  if (pud_section_has(pud, i))
                    fprintf(stdout, "%s\n", pud_section_to_string(i));
               }
          }
     }

end:
   free(out.file);
   pud_close(pud);
   war2_close(w2);
   return ret_status;
}
