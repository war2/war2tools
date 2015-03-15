#ifndef _PUDUTILS_H_
#define _PUDUTILS_H_

#include <pud.h>
#include <war2.h>
#include <getopt.h>

#include "../include/pud_private.h"

#define PROGRAM "pud"
#define VERSION  1.0

bool pud_jpeg_write(const char *file, int w, int h, const unsigned char *data);
bool pud_png_write(const char *file, int w, int h, const unsigned char *data);

bool pud_minimap_to_jpeg(Pud *pud, const char *file);
bool pud_minimap_to_png(Pud *pud, const char *file);

#endif /* ! _PUDUTILS_H_ */

