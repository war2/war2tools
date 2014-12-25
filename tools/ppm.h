#ifndef _PPM_H_
#define _PPM_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
   unsigned char r;
   unsigned char g;
   unsigned char b;
} Col;

Col *ppm_parse(const char *file, int *w_ret, int *h_ret);

#endif /* ! _PPM_H_ */

