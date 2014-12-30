#include "war2_private.h"

bool
war2_init(void)
{
   return pud_init();
}

void
war2_shutdown(void)
{
   pud_shutdown();
}

