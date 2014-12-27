#include "private.h"

bool
pud_defaults_set(Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_W, false);

   int i;

   // XXX Currently under heavy construction
   for (i = 0; i < 8; i++) pud->sgld.players[i] = 2000;
   for (i = 0; i < 7; i++) pud->sgld.unusable[i] = 2000;
   pud->sgld.neutral = 2000;

   for (i = 0; i < 8; i++) pud->slbr.players[i] = 1000;
   for (i = 0; i < 7; i++) pud->slbr.unusable[i] = 1000;
   pud->slbr.neutral = 1000;

   for (i = 0; i < 8; i++) pud->soil.players[i] = 1000;
   for (i = 0; i < 7; i++) pud->soil.unusable[i] = 1000;
   pud->soil.neutral = 1000;

   pud->default_allow = 1;
   pud->default_udta = 1;
   pud->default_ugrd = 1;

   //   pud->unit_data
   for (i = 0; i < 8; i++) pud->ai.players[i] = 0x00;

   return false; // XXX Set to true when done with
}

