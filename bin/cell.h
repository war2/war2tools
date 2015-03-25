#ifndef _CELL_H_
#define _CELL_H_

struct _Cell
{
   unsigned int tile : 9;                               // 9
   unsigned int unit_below : 7;                         // 16
   unsigned int unit_above : 7;                         // 23
   unsigned int orient_below : 3; /* 8 values */        // 26
   unsigned int orient_above : 3; /* 8 values */        // 29
   unsigned int player_below : 3; /* 8 values */        // 32
   unsigned int player_above : 3; /* 8 values */        // 35
   unsigned int spread_below : 2; /* 4 values */        // 37
   unsigned int spread_above : 2; /* 4 values */        // 39
   unsigned int anchor_below : 1;                       // 40
   unsigned int anchor_above : 1;                       // 41
};

Cell **cell_matrix_new(Editor *ed);
void cell_matrix_free(Cell **cells);

#endif /* ! _CELL_H_ */

