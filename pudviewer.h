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

bool jpeg_write(const char *file, int w, int h, unsigned char *data);

#include "pud.h"

#endif /* ! _PUDVIEWER_H_ */

