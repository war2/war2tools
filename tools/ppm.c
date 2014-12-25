#include "ppm.h"

static inline int
_str2int(const char *str)
{
   return strtol(str, NULL, 10);
}

Col *
ppm_parse(const char *file,
          int        *w_ret,
          int        *h_ret)
{
   FILE *f;
   Col *ptr = NULL;
   char buf[16];
   int c, k = 0, i = 0;
   int v;
   unsigned char status = 0;
   char comment = 0;
   int w, h;
   char col_st = 0;

   f = fopen(file, "r");
   if (!f)
     {
        fprintf(stderr, "*** Failed to open [%s]\n", file);
        return NULL;
     }

   while ((!feof(f)) && (!ferror(f)))
     {
        c = fgetc(f);

        if (c == '#')
          comment = 1;
        if (comment)
          {
             if (c == '\n')
               comment = 0;
             continue;
          }

        if ((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r'))
          {
             if (k != 0)
               {
                  buf[k] = 0;
                  if (status == 0)
                    {
                       /* Find PP3 */
                       if (strncmp(buf, "P3", 2) &&
                           strncmp(buf, "p3", 2))
                         {
                            fprintf(stderr, "*** Missing header\n");
                            goto end;
                         }
                       else
                         status++;
                    }
                  else if (status == 1)
                    {
                       w = _str2int(buf);
                       status++;
                    }
                  else if (status == 2)
                    {
                       h = _str2int(buf);
                       status++;
                    }
                  else if (status == 3)
                    {
                       v = _str2int(buf);
                       if (v != 255)
                         {
                            fprintf(stderr, "*** Only parse PPM with MAX = 255\n");
                            goto end;
                         }
                       status++;
                       ptr = calloc(w * h, sizeof(Col));
                       if (!ptr)
                         {
                            fprintf(stderr, "*** Failed to alloc\n");
                            goto end;
                         }
                    }
                  else
                    {
                       v = _str2int(buf);
                       if (col_st == 0)
                         ptr[i].r = v;
                       else if (col_st == 1)
                         ptr[i].g = v;
                       else
                         ptr[i].b = v;

                       if (++col_st >= 3)
                         {
                            col_st = 0;
                            i++;
                         }
                    }
                  k = 0;
               }
             continue;
          }
        buf[k++] = c;
     }

   if (w_ret) *w_ret = w;
   if (h_ret) *h_ret = h;

end:
   fclose(f);

   return ptr;

}

