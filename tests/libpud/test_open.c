#include "tests.h"
#include <pud.h>
#include <limits.h>

START_TEST(open)
{
   Pud *p;
   enum {
      F_NULL,
      F_EMPTY,
      F_GARBAGE,
      F_REAL
   };
   const char *files[] = {
      [F_NULL]    = NULL,
      [F_EMPTY]   = TESTS_SOURCE_DIR"/libpud/empty.pud",
      [F_GARBAGE] = TESTS_SOURCE_DIR"/libpud/garbage.pud",
      [F_REAL]    = TESTS_SOURCE_DIR"/libpud/cibola.pud",
   };
   unsigned int i;

   fail_if(pud_init() != PUD_TRUE);

   /* NULL file should NOT be accepted at all */
   p = pud_open(files[F_NULL], PUD_OPEN_MODE_RW);
   fail_if(p != NULL);

   /* EMPTY file cannot be READ from. WO is ok */
   p = pud_open(files[F_EMPTY], PUD_OPEN_MODE_R);
   fail_if(p != NULL);
   p = pud_open(files[F_EMPTY], PUD_OPEN_MODE_RW);
   fail_if(p != NULL);
   p = pud_open(files[F_EMPTY], PUD_OPEN_MODE_W);
   fail_if(p == NULL);
   pud_close(p);

   /* GARBAGE file cannot be READ from. WO is ok. */
   p = pud_open(files[F_GARBAGE], PUD_OPEN_MODE_R);
   fail_if(p != NULL);
   p = pud_open(files[F_GARBAGE], PUD_OPEN_MODE_RW);
   fail_if(p != NULL);
   p = pud_open(files[F_GARBAGE], PUD_OPEN_MODE_W);
   fail_if(p == NULL);
   pud_close(p);

   /* REAL file can be RO, RW, WO */
   p = pud_open(files[F_REAL], PUD_OPEN_MODE_R);
   fail_if(p == NULL);
   pud_close(p);
   p = pud_open(files[F_REAL], PUD_OPEN_MODE_RW);
   fail_if(p == NULL);
   pud_close(p);
   p = pud_open(files[F_REAL], PUD_OPEN_MODE_W);
   fail_if(p == NULL);
   pud_close(p);
   p = pud_open(files[F_REAL], PUD_OPEN_MODE_R | PUD_OPEN_MODE_NO_PARSE);
   fail_if(p == NULL);
   pud_close(p);

   pud_shutdown();
}
END_TEST

void
test_open(TCase *tc)
{
   tcase_add_test(tc, open);
}
