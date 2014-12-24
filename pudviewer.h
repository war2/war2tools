#ifndef _PUDVIEWER_H_
#define _PUDVIEWER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "debug.h"

#define PUDVIEWER_VERSION 1.0

uint8_t file_read_byte(FILE *file);
uint16_t file_read_word(FILE *file);
uint32_t file_read_long(FILE *file);

#ifndef __cplusplus
typedef unsigned char bool;
# define true  ((bool)1)
# define false ((bool)0)
#endif

typedef enum
{
   PUD_SECTION_TYPE     = 0,
   PUD_SECTION_VER      = 1,
   PUD_SECTION_DESC     = 2,
   PUD_SECTION_OWNR     = 3,
   PUD_SECTION_ERA      = 4,
   PUD_SECTION_ERAX     = 5,
   PUD_SECTION_DIM      = 6,
   PUD_SECTION_UDTA     = 7,
   PUD_SECTION_ALOW     = 8,
   PUD_SECTION_UGRD     = 9,
   PUD_SECTION_SIDE     = 10,
   PUD_SECTION_SGLD     = 11,
   PUD_SECTION_SLBR     = 12,
   PUD_SECTION_SOIL     = 13,
   PUD_SECTION_AIPL     = 14,
   PUD_SECTION_MTXM     = 15,
   PUD_SECTION_SQM      = 16,
   PUD_SECTION_OILM     = 17,
   PUD_SECTION_REGM     = 18,
   PUD_SECTION_UNIT     = 19
} Pud_Section;

typedef enum
{
   PUD_DIMENSIONS_UNDEFINED = 0,
   PUD_DIMENSIONS_32_32,
   PUD_DIMENSIONS_64_64,
   PUD_DIMENSIONS_96_96,
   PUD_DIMENSIONS_128_128
} Pud_Dimensions;


typedef struct _Pud Pud;



Pud *pud_new(const char *file);
void pud_free(Pud *pud);
int pud_parse(Pud *pud);
void pud_verbose_set(Pud *pud, int on);
bool pud_section_is_optional(Pud_Section sec);
uint32_t pud_go_to_section(Pud *pud, Pud_Section sec);
void pud_print(Pud *pud, FILE *stream);

bool pud_parse_type(Pud *pud);
bool pud_parse_ver(Pud *pud);
bool pud_parse_desc(Pud *pud);
bool pud_parse_era(Pud *pud);
bool pud_parse_dim(Pud *pud);
bool pud_parse_sgld(Pud *pud);
bool pud_parse_slbr(Pud *pud);
bool pud_parse_soil(Pud *pud);

#endif /* ! _PUDVIEWER_H_ */

