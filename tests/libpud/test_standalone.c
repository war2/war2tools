#include "tests.h"
#include <pud.h>
#include <limits.h>

START_TEST(init)
{
   fail_if(pud_init() != PUD_TRUE);
   pud_shutdown();
}
END_TEST

START_TEST(dimensions)
{
   unsigned int x, y, i, dim;

   fail_if(pud_init() != PUD_TRUE);

   pud_dimensions_to_size(PUD_DIMENSIONS_UNDEFINED, &x, &y);
   fail_if((x != 0) || (y != 0));

   pud_dimensions_to_size(PUD_DIMENSIONS_32_32, &x, &y);
   fail_if((x != 32) || (y != 32));

   pud_dimensions_to_size(PUD_DIMENSIONS_64_64, &x, &y);
   fail_if((x != 64) || (y != 64));

   pud_dimensions_to_size(PUD_DIMENSIONS_96_96, &x, &y);
   fail_if((x != 96) || (y != 96));

   pud_dimensions_to_size(PUD_DIMENSIONS_128_128, &x, &y);
   fail_if((x != 128) || (y != 128));

   for (i = 0; i < 100; i++) /* Try a few hundreds of invalid values */
     {
        dim = (rand() % (UINT_MAX - 5))  + 5;
        pud_dimensions_to_size(dim, &x, &y);
        fail_if((dim < 5) || (x != 0) || (y != 0));
     }

   pud_shutdown();
}
END_TEST

START_TEST(side_convert)
{
   unsigned int i;

   fail_if(pud_init() != PUD_TRUE);

   fail_if(pud_side_convert(0) != PUD_SIDE_HUMAN);
   fail_if(pud_side_convert(1) != PUD_SIDE_ORC);
   for (i = 2; i <= UINT8_MAX; i++)
     fail_if(pud_side_convert(i) != PUD_SIDE_NEUTRAL);

   pud_shutdown();
}
END_TEST

START_TEST(owner_convert)
{
   unsigned int i;

   fail_if(pud_init() != PUD_TRUE);

   fail_if(pud_owner_convert(0) != PUD_OWNER_PASSIVE_COMPUTER);
   fail_if(pud_owner_convert(1) != PUD_OWNER_COMPUTER);
   fail_if(pud_owner_convert(3) != PUD_OWNER_NOBODY);
   fail_if(pud_owner_convert(2) != PUD_OWNER_PASSIVE_COMPUTER);
   fail_if(pud_owner_convert(4) != PUD_OWNER_COMPUTER);
   fail_if(pud_owner_convert(5) != PUD_OWNER_HUMAN);
   fail_if(pud_owner_convert(6) != PUD_OWNER_RESCUE_PASSIVE);
   fail_if(pud_owner_convert(7) != PUD_OWNER_RESCUE_ACTIVE);
   for (i = 8; i <= UINT8_MAX; i++)
     fail_if(pud_owner_convert(i) != PUD_OWNER_PASSIVE_COMPUTER);

   pud_shutdown();
}
END_TEST

START_TEST(unit_valid_is)
{
   fail_if(pud_init() != PUD_TRUE);

   fail_if(pud_unit_valid_is(PUD_UNIT_NONE));
   fail_if(pud_unit_valid_is(-1));
   fail_if(pud_unit_valid_is(0x22));
   fail_if(pud_unit_valid_is(0x24));
   fail_if(pud_unit_valid_is(0x25));
   fail_if(pud_unit_valid_is(0x30));
   fail_if(pud_unit_valid_is(0x36));
   fail_if(!pud_unit_valid_is(0x00));
   fail_if(!pud_unit_valid_is(0x6c));
   fail_if(!pud_unit_valid_is(0x35));

   pud_shutdown();
}
END_TEST

void
test_standalone(TCase *tc)
{
   tcase_add_test(tc, init);
   tcase_add_test(tc, dimensions);
   tcase_add_test(tc, side_convert);
   tcase_add_test(tc, owner_convert);
   tcase_add_test(tc, unit_valid_is);
}
