#include "pud.h"

int
main(int    argc,
     char **argv)
{
   const char *file, *out;
   Pud_Bool chk;
   Pud *pud;

   if ((argc < 2) || (argc > 3))
     {
        fprintf(stderr, "*** Usage: %s <pud_file> [out=pud_file]\n", argv[0]);
        return 1;
     }
   file = argv[1];
   out = (argc == 3) ? argv[2] : file;
   
   pud = pud_open(file, PUD_OPEN_MODE_R | PUD_OPEN_MODE_W);
   if (!pud)
     {
        fprintf(stderr, "*** Failed to open pud file \"%s\"\n", file);
        return 2;
     }
   chk = pud_write(pud, out);
   if (!chk)
     {
        fprintf(stderr, "*** Failed to write pud to file \"%s\"\n", out);
        pud_close(pud);
        return 3;
     }
   pud_close(pud);

   return 0;
}
