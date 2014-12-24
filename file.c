#include "pudviewer.h"

uint8_t
file_read_byte(FILE *file)
{
   uint8_t byte;

   fread(&byte, sizeof(uint8_t), 1, file);
   return byte;
}

uint16_t
file_read_word(FILE *file)
{
   uint16_t word;

   fread(&word, sizeof(uint16_t), 1, file);
   return word;
}

uint32_t
file_read_long(FILE *file)
{
   uint32_t l;

   fread(&l, sizeof(uint32_t), 1, file);
   return l;
}

