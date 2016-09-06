#include "tests.h"
#include <pud.h>
#include <limits.h>

START_TEST(open)
{
   Pud *p;
   const char *files[] = {
      NULL,
      TESTS_BUILD_DIR"/empty.pud",
      TESTS_BUILD_DIR"/garbage.pud",
      TESTS_BUILD_DIR"/cibola.pud",
   };
   const unsigned int count = sizeof(files) / sizeof(files[0]);
   unsigned int i;

   fail_if(pud_init() != PUD_TRUE);

   /*
    * Test the result of pud_open() with:
    * - NULL file
    * - empty file
    * - rubbish file
    * - valid file
    */
   for (i = 0; i < count; i++)
     {
        const char *const f = files[i];

        p = pud_open(f, PUD_OPEN_MODE_R);
        fail_if(p != NULL);
        p = pud_open(f, PUD_OPEN_MODE_RW);
        fail_if(p != NULL);
        p = pud_open(f, PUD_OPEN_MODE_W);
        if (f) /* The NULL file will fail */
          {
             fail_if(p == NULL);
             pud_close(p);
          }
        else
          fail_if(p != NULL);
     }

   pud_shutdown();
}
END_TEST

void
test_open(TCase *tc)
{
   tcase_add_test(tc, open);
}
