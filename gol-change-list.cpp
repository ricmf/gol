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

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include "./array_list.h"
#include <time.h>
#include <cassert>


using namespace std;
typedef unsigned char cell_t;

cell_t ** allocate_board (int size) {
  cell_t ** board = (cell_t **) (malloc(sizeof(cell_t*)*size+2))+1;
  int   i;
  for (i=-1; i<size+1; i++)
    board[i] = (cell_t *) (malloc(sizeof(cell_t)*size+2))+1;
  return board;
}


void free_board (cell_t ** board, int size) {
  int     i;
  for (i=0; i<size; i++)
  free(board[i]);
  free(board);
}


/* return the number of on cells adjacent to the i,j cell */
int adjacent_to (cell_t ** board, int i, int j) {
  int count = 0;
  count+=board[i-1][j-1];
  count+=board[i-1][j];
  count+=board[i-1][j+1];
  count+=board[i][j-1];
  count+=board[i][j+1];
  count+=board[i+1][j-1];
  count+=board[i+1][j];
  count+=board[i+1][j+1];
  return count&0xf;
}


inline void push(structures::ArrayList<std::pair<int,int>>* new_live_cells, int i, int j, cell_t** newboard){
  if (newboard[i][j]>>4){
    new_live_cells->push_back(pair<int,int>{i,j});
    newboard[i][j] &= 0x1;
  }
}

void play (cell_t ** board, cell_t ** newboard, int size, structures::ArrayList<std::pair<int,int>>* old_live_cells
  , structures::ArrayList<std::pair<int,int>>* new_live_cells, int print) {
  int	pos, i, j, a;

  /* for each cell, apply the rules of Life */
  new_live_cells->clear();
  if (print){
      printf("size: %d\n", old_live_cells->size());
  }
  for (pos=0; pos<old_live_cells->size(); pos++){
      auto p = old_live_cells->at(pos);
      i = p.first; j = p.second;
      a = adjacent_to (board, i, j);
      //printf("i:%d j:%d newboard: %d  board:%d a: %d\n", i, j, newboard[i][j], board[i][j], a);
      if (a == 2){
        newboard[i][j] = board[i][j] | 0x10;
      } else if (a == 3) {
        newboard[i][j] = 0x11 ;
      } else {
        newboard[i][j] = 0x10;
      }
  }
  for (pos=0; pos<old_live_cells->size(); pos++){
      auto p = old_live_cells->at(pos);
      int i = p.first;
      int j = p.second;
      if ((board[i][j]& 0x1)) {
        push(new_live_cells, i, j, newboard);
        push(new_live_cells, i-1, j-1, newboard);
        push(new_live_cells, i-1, j, newboard);
        push(new_live_cells, i-1, j+1, newboard);
        push(new_live_cells, i, j-1, newboard);
        push(new_live_cells, i, j+1, newboard);
        push(new_live_cells, i+1, j-1, newboard);
        push(new_live_cells, i+1, j, newboard);
        push(new_live_cells, i+1, j+1, newboard);
      }
    }
}

/* print the life board */
void print (cell_t ** board, int size) {
  int	i, j;
  /* for each row */
  for (j=0; j<size; j++) {
    /* print each column position... */
    for (i=0; i<size; i++)
	      if (board[i][j] & 0x1){
	          printf ("■ ");
        }else{
	          printf ("□ ");
        }
    /* followed by a carriage return */
    printf ("\n");
  }
}

void fill_board(cell_t ** board, cell_t ** next, int size, int percentage_alive) {
  int	i, j;
  srand(time(NULL));
  for (j=0; j<size; j++) {
    for (i=0; i<size; i++){
	    board[i][j] = ((int)(rand()) %percentage_alive == 0? 1 : 0 ) | 0x10;
      next[i][j] = board[i][j];
      assert(board[i][j]>>4);
    }
  }
  for (j=-1; j<size+1; j++) {
    board[-1][j] = 0;
    board[j][-1] = 0;
    board[size][j] = 0;
    board[j][size] = 0;
    next[-1][j] = 0;
    next[j][-1] = 0;
    next[size][j] = 0;
    next[j][size] = 0;
  }
}

void read_file (FILE * f, cell_t ** board, cell_t ** board2, int size) {
  int	i, j;
  char	*s = (char *) malloc(size+10);

  /* read the first new line (it will be ignored) */
  fgets (s, size+10,f);

  /* read the life board */
  for (j=0; j<size; j++) {
    /* get a string */
    fgets (s, size+10,f);
    /* copy the string to the life board */
    for (i=0; i<size; i++){
      int a = (s[i] == 'x') |0x10;
      board[i][j] = a;
      //board2[i][j] = a;
    }
  }
  for (j=-1; j<size+1; j++) {
    board[-1][j] = 0;
    board[j][-1] = 0;
    board[size][j] = 0;
    board[j][size] = 0;
    //board2[-1][j] = 0;
    //board2[j][-1] = 0;
    //board2[size][j] = 0;
    //board2[j][size] = 0;
  }
}


// pra compilar usei o comando g++ gol-change-list.cpp -Ofast -std=gnu++14 -o output -mtune=native -funroll-loops
// e para rodar time cat input-big.in | ./output 11 7 8 0 1
// ele ignora os parametros 11 7 e 8, que são o size, steps, e um numero para escolher a proporcao de celulas vivas
// o 0 diz para não imprimir o quadro, e o 1 diz para ler o arquivo ao invés de gerar um quadro aleatório
// para rodar o input-little use time cat input-little.in | ./output 11 7 8 1 1
int main(int argc, char**argv){
  int size = std::atoi(argv[1]);
  int steps = std::atoi(argv[2]);
  int dead = std::atoi(argv[3]);
  int should_print = std::atoi(argv[4]);
  int use_file = std::atoi(argv[5]);
  cell_t ** prev;
  cell_t ** next;
  if (use_file){
    FILE    *f;
    f = stdin;
    fscanf(f,"%d %d", &size, &steps);
    prev = allocate_board (size);
    next = allocate_board (size);
    read_file (f, prev, next, size);
    fclose(f);
  } else {
    prev = allocate_board (size);
    next = allocate_board (size);
    fill_board(prev, next, size, dead);
  }
  cell_t ** tmp;

  int i;
  //print(prev,size);
  auto old_live_cells = new structures::ArrayList<std::pair<int,int>>{(size_t)size*size};
  auto new_live_cells = new structures::ArrayList<std::pair<int,int>>{(size_t)size*size};
  structures::ArrayList<std::pair<int,int>>* temp;
  for (int i = 0; i<size; ++i){
    for (int j = 0; j<size; ++j){
      old_live_cells->push_back(std::pair<int,int>{i,j});
    }
  }
  bool running = true;

  if (should_print){
      print(prev, size);
  }
  for (i=0; i<steps&&running; i++) {
    play (prev,next,size,  old_live_cells, new_live_cells, should_print);
    tmp = next;
    next = prev;
    prev = tmp;
    temp = old_live_cells;
    old_live_cells = new_live_cells;
    new_live_cells = temp;
    if (should_print){
        print(prev, size);
    }
  }
}
