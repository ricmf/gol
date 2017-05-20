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

void play (cell_t ** board, cell_t ** newboard, int size) {
  int	i, j, a;
  /* for each cell, apply the rules of Life */
  for (i=1; i<size+1; i++)
    for (j=1; j<size+1; j++) {
    a = adjacent_to (board, size, i, j);
    if (a == 2){
      newboard[i][j] = board[i][j];
    } else if (a == 3) {
      newboard[i][j] = 1 ;
    } else {
      newboard[i][j] = 0;
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
    printf ("%ls", board[i][j] ? '■' : '□');
    /* followed by a carriage return */
    printf ("\n");
  }
}

/* read a file into the life board */
void read_file (FILE * f, cell_t ** board, int size) {
  int	i, j;
  char	*s = (char *) malloc(size+10);

  /* read the first new line (it will be ignored) */
  fgets (s, size+10,f);

  /* read the life board */
  for (j=0; j<size; j++) {
    /* get a string */
    fgets (s, size+10,f);
    /* copy the string to the life board */
    for (i=0; i<size; i++)
    board[i+1][j+1] = s[i] == 'x';

  }
}

int main () {
  int size, steps;
  FILE    *f;
  f = stdin;
  fscanf(f,"%d %d", &size, &steps);
  cell_t ** prev = allocate_board (size+2);
  read_file (f, prev,size);
  fclose(f);
  cell_t ** next = allocate_board (size+2);
  cell_t ** tmp;
  int i;
  #ifdef DEBUG
  printf("Initial:\n");
  print(prev,size);
  #endif

  for (i=0; i<steps; i++) {
    play (prev,next,size);
    #ifdef DEBUG
    printf("%d ----------\n", i + 1);
    print (next,size);
    #endif
    tmp = next;
    next = prev;
    prev = tmp;
  }

#ifdef RESULT
  printf("Final:\n");
  print (prev,size);
#endif

  free_board(prev,size);
  free_board(next,size);
}
