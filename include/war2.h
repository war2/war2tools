#ifndef _WAR2_H_
#define _WAR2_H_
#ifdef __cplusplus
extern "C" <%
#endif

#include "pud.h"

bool war2_init(void);
void war2_shutdown(void);

typedef struct _War2_Data War2_Data;

struct _War2_Data
{
   unsigned char *mem_map;
   size_t         mem_map_size;
   unsigned char *ptr;

   uint32_t     magic;
   uint16_t     entries_count;

   unsigned int verbose;
};

War2_Data *war2_open(const char *file, int verbose);
void war2_close(War2_Data *w2);
void war2_verbosity_set(War2_Data *w2, int level);


#ifdef __cplusplus
%>
#endif
#endif /* ! _WAR2_H_ */

