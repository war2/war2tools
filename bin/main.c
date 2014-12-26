#include "war2edit.h"

EAPI_MAIN int
elm_main(int    argc,
         char **argv)
{
   log_init();
   editor_init();

   editor_new(PUD_ERA_FOREST, PUD_DIMENSIONS_128_128);
   elm_run();
   elm_shutdown();

   editor_shutdown();
   log_shutdown();

   return 0;
}
ELM_MAIN()

