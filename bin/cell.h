#ifndef _CELL_H_
#define _CELL_H_

struct _Cell
{
   unsigned int tile : 9;
   unsigned int unit : 7;
   unsigned int orient : 4;
   unsigned int player : 3;
   unsigned int anchor : 1;
};

Cell **cell_matrix_new(Editor *ed);
void cell_matrix_free(Cell **cells);

#endif /* ! _CELL_H_ */

