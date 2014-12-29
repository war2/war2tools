#include "pudutils.h"

static bool
_char_valid_is(char c)
{
   return (((c >= 'A') && (c <= 'Z')) || (c == ' '));
}

bool
sections_list(Pud  *pud,
              FILE *stream)
{
   FILE *f = pud->file;
   char buf[8], c;
   bool valid[4];
   int vc = 0;
   int i;
   int verified = 0, unchecked = 0;
   char check;

   fread(buf, sizeof(char), 4, f);
   PUD_CHECK_FERROR(f, false);
   buf[4] = 0;

   for (i = 0; i < 4; i++)
     {
        valid[i] = _char_valid_is(buf[i]);
        vc += valid[i];
     }

   while (!feof(f))
     {
        if (vc == 4)
          {
             if (pud_section_exists(buf))
               {
                  verified++;
                  check = ' ';
               }
             else
               {
                  unchecked++;
                  check = '~';
               }
             fprintf(stream, "%c  %s\n", check, buf);
          }

        if (valid[0] == true) vc--;
        memmove(&(buf[0]), &(buf[1]), 3 * sizeof(char));
        memmove(&(valid[0]), &(valid[1]), 3 * sizeof(char));
        fread(&c, sizeof(char), 1, f);
        PUD_CHECK_FERROR(f, false);

        valid[3] = _char_valid_is(c);
        vc += valid[3];
        buf[3] = c;
     }

   fprintf(stream,
           "Valid....: %i\n"
           "Unsure...: %i\n"
           "Total....: %i\n", verified, unchecked, verified + unchecked);

   return true;
}

