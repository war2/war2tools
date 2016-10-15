/**
 * @file pud.c
 */
/*
 * Compile (within a POSIX shell):
 *
 * gcc -o pud-description pud.c $(pkg-config --cflags --libs pud)
 *
 * Run:
 *
 * ./pud-description /path/to/file.pud
 */

#include <pud.h>
#include <stdio.h>

int
main(int    argc,
     char **argv)
{
   int ret = 1;
   Pud *pud;
   const char *const file = argv[1];
   const char *desc;

   /* Init libpud. */
   if (!pud_init())
     {
        fprintf(stderr, "*** Failed to init libpud\n");
        return 1;
     }

   /* Open a file read-only */
   pud = pud_open(file, PUD_OPEN_MODE_R);

   /* Opening a file might fail if the file is invalid */
   if (!pud)
     {
        fprintf(stderr, "*** Failed to open \"%s\"\n", file);
        goto end;
     }

   /* Get the description of the pud file */
   desc = pud_description_get(pud);
   if (!desc)
     {
        fprintf(stderr, "*** Failed to get description\n");
        goto end;
     }

   printf("PUD Description of \"%s\" is \"%s\"\n", file, desc);

   ret = 0;
end:
   /* Close the previously open pud file */
   pud_close(pud);

   /* Clean-up: ensure libpud resources are released */
   pud_shutdown();
   return ret;
}
