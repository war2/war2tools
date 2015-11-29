#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <Eet.h>

size_t
_file_size_get(FILE *f)
{
   size_t size;

   fseek(f, 0L, SEEK_END);
   size = ftell(f);
   fseek(f, 0L, SEEK_SET);

   return size;
}

int
main(int    argc,
     char **argv)
{
   FILE *in;
   Eet_File *out;
   uint8_t buf[4];
   unsigned char *p;
   size_t mem_len;
   uint16_t edge;
   unsigned int i;
   const uint16_t word_zero = 0;

   if (argc != 4)
     {
        fprintf(stderr, "Usage: %s <input.data> <output.eet> <key>\n", argv[0]);
        return 1;
     }

   eet_init();

   in = fopen(argv[1], "rb");
   if (!in)
     {
        fprintf(stderr, "*** Failed to open \"%s\"\n", argv[1]);
        return 2;
     }

   mem_len = _file_size_get(in);
   edge = sqrt(mem_len /4);
   mem_len += 8;
   p = malloc(mem_len);
   if (!p)
     {
        fprintf(stderr, "*** Failed to malloc of size %zu\n", mem_len);
        fclose(in);
        return 2;
     }

   out = eet_open(argv[2], EET_FILE_MODE_READ_WRITE);
   if (!out)
     {
        fprintf(stderr, "*** Failed to open \"%s\"\n", argv[2]);
        fclose(in);
        free(p);
        return 2;
     }

   memcpy(&(p[0]), &word_zero, sizeof(uint16_t));
   memcpy(&(p[2]), &word_zero, sizeof(uint16_t));
   memcpy(&(p[4]), &edge,      sizeof(uint16_t));
   memcpy(&(p[6]), &edge,      sizeof(uint16_t));
   i = 4 * sizeof(uint16_t);
   while (fread(buf, sizeof(uint8_t), 4, in) == sizeof(buf))
     {
        memcpy(&(p[i++]), &(buf[2]), sizeof(uint8_t));
        memcpy(&(p[i++]), &(buf[1]), sizeof(uint8_t));
        memcpy(&(p[i++]), &(buf[0]), sizeof(uint8_t));
        memcpy(&(p[i++]), &(buf[3]), sizeof(uint8_t));
     }

   eet_write(out, argv[3], p, mem_len, 0);
   fclose(in);
   eet_close(out);
   eet_shutdown();

   return 0;
}

