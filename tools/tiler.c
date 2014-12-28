#include <pud.h>

static void
_usage(void)
{
   fprintf(stderr, "*** Usage: tiler <era> <out.pud>\n");
   fprintf(stderr, "***   Era: forest,winter,wasteland,swamp\n");
}

int
main(int    argc,
     char **argv)
{
   const char *era, *file;
   Pud_Era pud_era;
   Pud *pud;
   bool chk;

   /* Getopt */
   if (argc != 3)
     {
        _usage();
        return 1;
     }

   era = argv[1];
   file = argv[2];

   if (!strcasecmp(era, "forest"))
     pud_era = PUD_ERA_FOREST;
   else if (!strcasecmp(era, "winter"))
     pud_era = PUD_ERA_WINTER;
   else if (!strcasecmp(era, "wasteland"))
     pud_era = PUD_ERA_WASTELAND;
   else if (!strcasecmp(era, "swamp"))
     pud_era = PUD_ERA_SWAMP;
   else
     {
        _usage();
        return 1;
     }

   pud = pud_open(file, PUD_OPEN_MODE_R);
   if (!pud)
     {
        fprintf(stderr, "*** Failed to open [%s]\n", file);
        return 2;
     }
   chk = pud_parse(pud);
   if (!chk)
     {
        fprintf(stderr, "*** Fail to parse\n");
        pud_close(pud);
     }

   chk = pud_reopen(pud, "out.pud", PUD_OPEN_MODE_W);
   if (!chk)
     {
        fprintf(stderr, "*** Failed to reopen\n");
        pud_close(pud);
     }

   chk = pud_write(pud);
   if (!chk) fprintf(stderr, "*** Fail!\n");

   //   pud_defaults_set(pud);
   //   pud_era_set(pud, pud_era);

   pud_close(pud);

   return 0;
}

