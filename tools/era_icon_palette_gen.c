#include <pud.h>

static void
_usage(FILE *f)
{
   fprintf(f,
           "Usage: era_icon_palette_gen <EXTRACTED ICON> <COLORIZED COLOR>\n");
}

int
main(int    argc,
     char **argv)
{
   const char *f1, *f2;

   if (argc != 3)
     {
        fprintf(stderr, "*** Not enough arguments\n");
        _usage(stderr);
        return 1;
     }

   f1 = argv[1];
   f2 = argv[2];


   return 0;
}
