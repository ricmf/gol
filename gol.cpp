/*
* The Game of Life
*
* a cell is born, if it has exactly three neighbours
* a cell dies of loneliness, if it has less than two neighbours
* a cell dies of overcrowding, if it has more than three neighbours
* a cell survives to the next generation, if it does not die of loneliness
* or overcrowding
*
* In this version, a 2D array of ints is used.  A 1 cell is on, a 0 cell is off.
* The game plays a number of steps (given by the input), printing to the screen each time.  'x' printed
* means on, space means off.
*
*/
#include <stdio.h>
#include <stdlib.h>
#include "./array_list.h"
#include <time.h>

typedef unsigned char cell_t;

cell_t ** allocate_board (int size) {
  cell_t ** board = (cell_t **) malloc(sizeof(cell_t*)*size);
  int	i;
  for (i=0; i<size; i++)
  board[i] = (cell_t *) malloc(sizeof(cell_t)*size);
  return board;
}

void free_board (cell_t ** board, int size) {
  int     i;
  for (i=0; i<size; i++)
  free(board[i]);
  free(board);
}


/* return the number of on cells adjacent to the i,j cell */
int adjacent_to (cell_t ** board, int size, int i, int j) {
  int count = 0;
  count+=board[i-1][j-1];
  count+=board[i-1][j];
  count+=board[i-1][j+1];
  count+=board[i][j-1];
  count+=board[i][j+1];
  count+=board[i+1][j-1];
  count+=board[i+1][j];
  count+=board[i+1][j+1];
  return count;
}

void play (cell_t ** board, cell_t ** newboard, int size, structures::ArrayList<std::pair<int,int>>* old_live_cells, structures::ArrayList<std::pair<int,int>>* new_live_cells) {
  int	pos, i, j, a;
  /* for each cell, apply the rules of Life */
  new_live_cells->clear();
  for (i=1; i<size; i++)
    for (j=1; j<size; j++) {
      newboard[i][j] = next_state[board[i][j]];
    }
  }
  for (i=1; i<size; i++)
    for (j=1; j<size; j++) {
      int nb = newboard[i][j];
      int b = board[i][j];
      if (nb & 0x1  != b & 0x1){//uppermost leftmost bit
        if (nb & 0x1){
          newboard[i-1][j-1] += 0x20000000;
          newboard[i-1][j] += 0x02000000;
          newboard[i][j-1] + 0x10;
        }
      }
      if (nb & (0x1 <<3) != b & (0x1 <<3)){//uppermost leftmiddle bit
        newboard[i-1][j] = newboard[i-1][j] ^ (0x1 <<3);
      }
      if (nb & (0x1 <<5) != b & (0x1 <<5)){//uppermost leftmiddle bit
        newboard[i-1][j] ^= (0x1 <<5);
      }
      if (nb & (0x1 <<7) != b & (0x1 <<7)){//uppermost leftmiddle bit
        if (nb & 0x1){
            newboard[i-1][j-1] += 0x20000000;
            newboard[i-1][j] += 0x02000000;
            newboard[i][j-1] + 0x10;
        }
      }
      if (nb & (0x1 <<10) != b & (0x1 <<10)){//uppermost leftmiddle bit
        newboard[i-1][j] ^= (0x1 <<10);
      }
      if (nb & (0x1 <<12) != b & (0x1 <<12)){//uppermost leftmiddle bit
        newboard[i-1][j] ^= (0x1 <<12);
      }
      if (nb & (0x1 <<14) != b & (0x1 <<14)){//uppermost leftmiddle bit
        newboard[i-1][j] ^= (0x1 <<14);
      }
      if (nb & (0x1 <<16) != b & (0x1 <<16)){//uppermost leftmiddle bit
        newboard[i-1][j] ^= (0x1 <<16);
      }
      if (nb & (0x1 <<18) != b & (0x1 <<18)){//uppermost leftmiddle bit
        newboard[i-1][j] ^= (0x1 <<18);
      }
      if (nb & (0x1 <<20) != b & (0x1 <<20)){//uppermost leftmiddle bit
        newboard[i-1][j] ^= (0x1 <<20);
      }
      if (nb & (0x1 <<22) != b & (0x1 <<22)){//uppermost leftmiddle bit
        if (nb & 0x1){
            newboard[i-1][j-1] += 0x20000000;
            newboard[i-1][j] += 0x02000000;
            newboard[i][j-1] + 0x10;
        }
      }
      if (nb & (0x1 <<25) != b & (0x1 <<25)){//uppermost leftmiddle bit
        newboard[i-1][j] ^= (0x1 <<25);
      }
      if (nb & (0x1 <<27) != b & (0x1 <<27)){//uppermost leftmiddle bit
        newboard[i-1][j] ^= (0x1 <<27);
      }
      if (nb & (0x1 <<29) != b & (0x1 <<29)){//uppermost leftmiddle bit
        if (nb & 0x1){
            newboard[i-1][j-1] += 0x20000000;
            newboard[i-1][j] += 0x02000000;
            newboard[i][j-1] + 0x10;
        }
      }
    }
  }
}

/* print the life board */
void print (cell_t ** board, int size) {
  int	i, j;
  /* for each row */
  for (j=1; j<size+1; j++) {
    /* print each column position... */
    for (i=1; i<size+1; i++)
	if (board[i][j]){
	    printf ("■ ");
        }else{
	    printf ("□ ");
        }
    /* followed by a carriage return */
    printf ("\n");
  }
}

void fill_board(cell_t ** board, int size) {
  int	i, j;
  srand(time(NULL));
  /* for each row */
  for (j=1; j<size+1; j++) {
    /* print each column position... */
    for (i=1; i<size+1; i++)
	    board[i][j] = (int)(rand()) & 1;
    /* followed by a carriage return */
    printf ("\n");
  }
}

int main(int argc, char**argv){
  int size = std::atoi(argv[1])/3;
  int steps = std::atoi(argv[2]);
  cell_t ** prev = allocate_board (size+2);
  fill_board(prev, size);
  cell_t ** next = allocate_board (size+2);
  cell_t ** tmp;
  int i;
  printf("Initial:\n");
  print(prev,size);
  auto old_live_cells = new structures::ArrayList<std::pair<int,int>>{size*size*8};
  auto new_live_cells = new structures::ArrayList<std::pair<int,int>>{size*size*8};
  structures::ArrayList<std::pair<int,int>>* temp;
  for (int i = 1; i<size+1; ++i){
    for (int j = 1; j<size+1; ++j){
      old_live_cells->push_back(std::pair<int,int>{i,j});
    }
  }
  for (i=0; i<steps; i++) {
    play (prev,next,size,  old_live_cells, new_live_cells);
    #ifdef DEBUG
    printf("%d ----------\n", i + 1);
    print (next,size);
    #endif
    tmp = next;
    next = prev;
    prev = tmp;
    temp = old_live_cells;
    old_live_cells = new_live_cells;
    new_live_cells = temp;
  }
  printf("Final:\n");
  print (prev,size);
  free_board(prev,size);
  free_board(next,size);
}
