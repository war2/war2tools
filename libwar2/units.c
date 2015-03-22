/*
 * units.c
 * libwar2
 *
 * Copyright (c) 2015 Jean Guyomarc'h
 */

#include "war2_private.h"

static Pud_Bool
_units_entries_parse(War2_Data              *w2,
                     War2_Units_Descriptor  *ud,
                     const unsigned int     *entries,
                     War2_Units_Decode_Func  func)
{
   unsigned char *ptr;
   size_t size;

   /* If no callback has been specified, do nothing */
   if (!func)
     {
        WAR2_VERBOSE(w2, 1, "Warning: No callback specified.");
        return PUD_TRUE;
     }

   /* Palette */
   ptr = war2_palette_extract(w2, entries[0]);
   if (!ptr) DIE_RETURN(PUD_FALSE, "Failed to get palette");
   war2_palette_convert(ptr, ud->palette);
   free(ptr);

   ptr = war2_entry_extract(w2, entries[1], &size);

   return PUD_TRUE;
}


War2_Units_Descriptor *
war2_units_decode(War2_Data              *w2,
                  Pud_Player              player_color,
                  Pud_Side                race,
                  War2_Units_Decode_Func  func)
{
   War2_Units_Descriptor *ud;
   const unsigned int entries[] = { 2, 33 };

   /* Alloc */
   ud = calloc(1, sizeof(*ud));
   if (!ud) DIE_RETURN(NULL, "Failed to allocate memory");
   ud->race = race;
   ud->color = player_color;

   _units_entries_parse(w2, ud, entries, func);

   return ud;
}

