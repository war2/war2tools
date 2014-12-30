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

War2_Data *
war2_open(const char *file,
          int         verbosity)
{
   War2_Data *w2;

   /* Allocate memory and set verbosity */
   w2 = calloc(1, sizeof(War2_Data));
   if (!w2) DIE_GOTO(err, "Failed to allocate memory");
   war2_verbosity_set(w2, verbosity);

   /* Map file */
   w2->mem_map = pud_mmap(file, &(w2->mem_map_size));
   if (!w2->mem_map) DIE_GOTO(err_free, "Failed to map file");
   w2->ptr = w2->mem_map;

   /* Read magic */
   w2->magic = READ32(w2);
   switch (w2->magic)
     {
      case 0x00000019: // Handled
         WAR2_VERBOSE(w2, 1, "File [%s] has magic [0x%08x]", file, w2->magic);
         break;

      default:
         ERR("Unknown or (yet) unsupported magic [0x%08x]", w2->magic);
         goto err_close;
     }

   /* Get the entries */
   w2->entries_count = READ16(w2);
   WAR2_VERBOSE(w2, 1, "File [%s] has [%u] entries", file, w2->entries_count);

   return w2;

err_close:
   pud_munmap(w2->mem_map, w2->mem_map_size);
err_free:
   free(w2);
err:
   return NULL;
}

void
war2_close(War2_Data *w2)
{
   if (!w2) return;
   pud_munmap(w2->mem_map, w2->mem_map_size);
   free(w2);
}

void
war2_verbosity_set(War2_Data *w2,
                   int        level)
{
   if (w2) w2->verbose = level;
}

