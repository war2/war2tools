#include "war2edit.h"

int _war2edit_log_dom = -1;

Eina_Bool
log_init(void)
{
   _war2edit_log_dom = eina_log_domain_register("war2edit", EINA_COLOR_GREEN);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(_war2edit_log_dom < 0, EINA_FALSE);

   return EINA_TRUE;
}

void
log_shutdown(void)
{
   if (_war2edit_log_dom >= 0)
     {
        eina_log_domain_unregister(_war2edit_log_dom);
        _war2edit_log_dom = -1;
     }
}

