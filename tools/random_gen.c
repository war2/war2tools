#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int
main(int    argc,
     char **argv)
{
   FILE *f;
   char buf[512];
   uint16_t tile, mask;
   enum {
      ST_START,
      ST_SOLID,
      ST_BOUNDRY
   } state = ST_START;
   uint8_t rands[0xf];
   unsigned int k = 0, i;
   uint16_t kase;

   if (argc != 2)
     {
        fprintf(stderr, "*** Usage: %s <list>\n", argv[0]);
        fprintf(stderr,
                "*** Generate the list with something like:\n"
                "***   1) ./tools/extract_sprites path/to/maindat.war\n"
                "***   2) ls -1 ./tiles/png/winter > winter.list\n");
        return 1;
     }
   f = fopen(argv[1], "r");
   if (!f)
     {
        fprintf(stderr, "*** Failed to open \"%s\"\n", argv[1]);
        return 2;
     }

   while (fgets(buf, sizeof(buf), f))
     {
        buf[6] = 0; /* Keep 0x???? only */
        tile = strtol(buf, NULL, 16);

        switch (state)
          {
           case ST_START:
             printf("if ((tile & 0xff00) == 0x0000) /* Solid */\n");
             printf("  {\n");
             printf("     switch (tile & 0x00f0)\n");
             printf("       {\n");
             state = ST_SOLID;
             mask = 0x00f0;
             kase = tile & mask;
             break;

           case ST_SOLID:
             if ((0x0f00 & tile) != 0x0000)
               {
                  printf("    default:\n");
                  printf("       fprintf(stderr, \"*** Invalid tile %%x\\n\", tile);\n");
                  printf("       return 0x0000;\n");
                  printf("      }\n");
                  printf("  }\n");
                  printf("else /* Boundry */\n");
                  printf("  {\n");
                  printf("     switch (tile & 0x0ff0)\n");
                  printf("        {\n");
                  mask = 0x0ff0;
                  kase = tile & mask;
                  state = ST_BOUNDRY;
                  k = 0;
               }
             break;

           case ST_BOUNDRY:
             break;
          }

        if ((tile & mask) != kase)
          {
             printf(" case 0x%04x:\n", kase);
             printf("    GEN(%u", k);
             for (i = 0; i < k; ++i)
               printf(", 0x%1x", rands[i]);
             printf("); break;\n");
             k = 0;
             kase = (tile & mask);
          }

        rands[k++] = (tile & 0x000f);
     }

   printf("    default:\n");
   printf("       fprintf(stderr, \"*** Invalid tile %%x\\n\", tile);\n");
   printf("       return 0x0000;\n");
   printf("      }\n");
   printf("  }\n");

   fclose(f);
   return 0;
}

