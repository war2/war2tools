#ifndef _CURSOR_H_
#define _CURSOR_H_

Eina_Bool cursor_add(Editor *ed);
void cursor_enable(Editor *ed);
void cursor_disable(Editor *ed);
void cursor_resize(Editor *ed, int w, int h);
void cursor_move(Editor *ed, int x, int y);

#endif /* ! _CURSOR_H_ */

