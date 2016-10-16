/**
 * @file war2.c
 */
/*
 * Compile (within a POSIX shell):
 *
 * gcc -o war2-dragon-extract war2.c $(pkg-config --cflags --libs war2)
 *
 * Run:
 *
 * ./war2-dragon-extract /path/to/MAINDAT.WAR
 */

#include <war2.h>
#include <stdio.h>
#include <inttypes.h>

static void
_sprites_cb(void                          *data,
            const Pud_Color               *img,
            int                            x,
            int                            y,
            unsigned int                   w,
            unsigned int                   h,
            const War2_Sprites_Descriptor *ud,
            uint16_t                       sprite_id)
{
   char file[4096];
   const char *const name = data; /* Get the data passed to the callback */
   Pud_Bool chk;

   /* Sprite will be saved as "<DATA>_<SPRITE_ID>.png" */
   snprintf(file, sizeof(file), "%s_%02"PRIu16".png", name, sprite_id);
   file[sizeof(file) - 1] = '\0';

   /* Write the sprite in a PNG file */
   chk = war2_png_write(file, w, h, (const unsigned char *)img);
   if (!chk)
     fprintf(stderr, "*** Failed to save sprite in \"%s\"\n", file);
   else
     printf(" (sprite) %s\n", file);
}

int
main(int    argc,
     char **argv)
{
   int ret = 1;
   War2_Data *w2;
   Pud_Bool chk;
   const char *const file = argv[1];

   /* Init libwar2. */
   if (!war2_init())
     {
        fprintf(stderr, "*** Failed to init libwar2\n");
        return 1;
     }

   /* Open MAINDAT.WAR */
   w2 = war2_open(file);

   /* Opening a file might fail if the file is invalid */
   if (!w2)
     {
        fprintf(stderr, "*** Failed to open \"%s\"\n", file);
        goto end;
     }

   /* Extract the entry 36 (DRAGON) with the violet color */
   chk = war2_sprites_decode_entry(w2, PUD_PLAYER_VIOLET, 36, _sprites_cb, "dragon");
   if (!chk)
     {
        fprintf(stderr, "*** An error occured while extracting sprites\n");
        goto end;
     }
 
   ret = 0;
end:
   /* Close the previously open Warcraft II data file */
   war2_close(w2);

   /* Clean-up: ensure libwar2 resources are released */
   war2_shutdown();
   return ret;
}
