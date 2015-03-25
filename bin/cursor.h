#ifndef _CURSOR_H_
#define _CURSOR_H_

Eina_Bool cursor_add(Editor *ed);
void cursor_enable(Editor *ed);
void cursor_disable(Editor *ed);
void cursor_size_set(Editor *ed, int w, int h);
void cursor_size_get(Editor *ed, int *w, int *h);
void cursor_pos_set(Editor *ed, int x, int y);
void cursor_pos_get(Editor *ed, int *x, int *y);
void cursor_hide(Editor *ed);
void cursor_show(Editor *ed);

#endif /* ! _CURSOR_H_ */

