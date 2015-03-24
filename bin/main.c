#include "war2edit.h"

EAPI_MAIN int
elm_main(int    argc EINA_UNUSED,
         char **argv EINA_UNUSED)
{
   elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);
 //  elm_language_set("");
  // elm_app_compile_bin_dir_set(PACKAGE_COMPILE_BIN_DIR);
  // elm_app_compile_data_dir_set(PACKAGE_COMPILE_DATA_DIR);
  // elm_app_info_set(elm_main, "war2edit", "themes/default.edj");

   log_init();
   texture_init();
   editor_init();

   editor_new();
   elm_run();
   elm_shutdown();

   editor_shutdown();
   texture_shutdown();
   log_shutdown();

   return 0;
}
ELM_MAIN()

