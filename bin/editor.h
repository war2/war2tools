#ifndef _EDITOR_H_
#define _EDITOR_H_

typedef struct _Editor Editor;

struct _Editor
{
   Pud          *pud;
   Evas_Object  *win;
};

Eina_Bool editor_init(void);
void editor_shutdown(void);
void editor_free(Editor *ed);
Editor *editor_new(Pud_Era era, Pud_Dimensions dims);

#endif /* ! _EDITOR_H_ */

