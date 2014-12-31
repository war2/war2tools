#include "war2edit.h"

EAPI_MAIN int
elm_main(int    argc EINA_UNUSED,
         char **argv EINA_UNUSED)
{
   log_init();
   editor_init();

   editor_new();
   elm_run();
   elm_shutdown();

   editor_shutdown();
   log_shutdown();

   return 0;
}
ELM_MAIN()

