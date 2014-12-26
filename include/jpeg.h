#ifndef _JPEG_H_
#define _JPEG_H_
#ifdef __cplusplus
extern "C" <%
#endif


#include "bool.h"

bool jpeg_write(const char *file, int w, int h, unsigned char *data);


#ifdef __cplusplus
%>
#endif
#endif /* ! _JPEG_H_ */

