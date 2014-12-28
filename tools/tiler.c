#include <pud.h>
#include "../include/debug.h"

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
   int idx;

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

   pud_init();

   pud = pud_open(file, PUD_OPEN_MODE_W);
   if (!pud) DIE_RETURN(2, "Failed to open [%s]", file);

   chk = pud_defaults_set(pud);
   if (!chk) DIE_RETURN(3, "Failed to set defaults");

   pud_era_set(pud, pud_era);
   pud_dimensions_set(pud, PUD_DIMENSIONS_128_128);
   pud_version_set(pud, 0x11);
   pud_description_set(pud, "A description");
   pud_tag_set(pud, 0);

   idx = pud_unit_add(pud, 1, 1, PUD_PLAYER_RED, PUD_UNIT_HUMAN_START, 1);
   if (idx < 0) DIE_RETURN(1, "Failed to add unit");
   idx = pud_unit_add(pud, 1, 1, PUD_PLAYER_RED, PUD_UNIT_INFANTRY, 1);
   if (idx < 0) DIE_RETURN(1, "Failed to add unit");
   idx = pud_unit_add(pud, 1, 2, PUD_PLAYER_BLUE, PUD_UNIT_ORC_START, 1);
   if (idx < 0) DIE_RETURN(1, "Failed to add unit");
   idx = pud_unit_add(pud, 1, 2, PUD_PLAYER_BLUE, PUD_UNIT_PEON, 1);
   if (idx < 0) DIE_RETURN(1, "Failed to add unit");

   chk = pud_write(pud);
   if (!chk) DIE_RETURN(4, "Failed to write pud");

   pud_close(pud);

   pud_shutdown();

   return 0;
}

