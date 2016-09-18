#include "pud.h"
#include <inttypes.h>

static inline uint_fast32_t
_bit(uint_fast32_t shift)
{
   return ((uint_fast32_t)1 << shift);
}

int
main(int    argc,
     char **argv)
{
   Pud *pud;
   const char *file;
   Pud_Player pl;
   uint_fast32_t idx;
   Pud_Bool set, chk;
   unsigned int i;
   int ret = EXIT_FAILURE;

   if (argc != 5)
     {
        fprintf(stderr,
                "*** Usage: %s <file.pud> <player ID> <upgrade index> {1,0}\n",
                argv[0]);
        goto end;
     }

   file = argv[1];
   pl = atoi(argv[2]);
   idx = (uint_fast32_t)atoi(argv[3]);
   set = !!atoi(argv[4]);


   if ((unsigned)pl >= PUD_PLAYER_YELLOW)
     {
        fprintf(stderr, "*** Invalid player ID 0x%x\n", pl);
        goto end;
     }

   if (idx >= 31)
     {
        fprintf(stderr, "*** Index must be between 0 and 31 (both inclusive)\n");
        goto end;
     }

   pud = pud_open(file, PUD_OPEN_MODE_RW);
   if (!pud)
     {
        fprintf(stderr, "*** Failed to open pud file \"%s\"\n", file);
        goto end;
     }

   if (set)
     pud->up_alow.players[pl] |= _bit(idx);
   else
     pud->up_alow.players[pl] &= ~_bit(idx);

   chk = pud_write(pud, file);
   if (!chk)
     {
        fprintf(stderr, "*** Failed to save pud at path \"%s\"\n", file);
        goto free;
     }

   for (i = 0; i < 8; i++)
     {
        printf("Player %i (%s) => 0x%"PRIx32"\n",
               i, pud_color2str(i), pud->up_alow.players[i]);
     }


   ret = EXIT_SUCCESS;
free:
   pud_close(pud);
end:
   return ret;
}
