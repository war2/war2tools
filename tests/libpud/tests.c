#include "tests.h"

static const Efl_Test_Case etc[] = {
     { "Standalone", test_standalone },
     { "Open", test_open },
     { NULL, NULL }
};

int
main(int          argc,
     const char **argv)
{
   int failed_count;

   if (!_efl_test_option_disp(argc, argv, etc))
     return 0;

   failed_count = _efl_suite_build_and_run(argc - 1, argv + 1,
                                           "libpud", etc);

   return (failed_count == 0) ? 0 : -1;
}
