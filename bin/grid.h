#ifndef _GRID_H_
#define _GRID_H_


struct _Grid_Cell
{
   unsigned int tile : 9;
   unsigned int unit : 7;
   unsigned int orient : 4;
   unsigned int player : 3;
   unsigned int anchor : 1;
};


Eina_Bool grid_add(Editor *ed);
void grid_del(Editor *ed);

#endif /* ! _GRID_H_ */

