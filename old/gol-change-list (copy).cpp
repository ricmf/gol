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


char next_state[64];

void free_board (cell_t ** board, int size) {
  int     i;
  for (i=0; i<size; i++)
  free(board[i]);
  free(board);
}


/* return the number of on cells adjacent to the i,j cell */
int adjacent_to (cell_t ** board, int i, int j) {
  int count = 0;
  count+=board[i-1][j-1]>>4;
  count+=board[i-1][j]>>4;
  count+=board[i-1][j+1]>>4;
  count+=board[i][j-1]>>4;
  count+=board[i][j+1]>>4;
  count+=board[i+1][j-1]>>4;
  count+=board[i+1][j]>>4;
  count+=board[i+1][j+1]>>4;
  return count;
}

inline void copy_borders(cell_t** board, int size){
  for (int i=0; i<size+1; i++){
    board[i][0] = board[i][size];
    board[i][size+1] = board[i][1];
    board[0][i] = board[size][i];
    board[size+1][i] = board[1][i];
  }
}

void play (cell_t ** board, cell_t ** newboard, int size, structures::ArrayList<std::pair<int,int>>* old_live_cells
  , structures::ArrayList<std::pair<int,int>>* new_live_cells) {
  int	pos, i, j, a;

  /* for each cell, apply the rules of Life */
  new_live_cells->clear();
  //printf("%d\n",old_live_cells->size());
  for (pos=0; pos<old_live_cells->size(); pos++){
      auto p = old_live_cells->at(pos);
      newboard[i][j] = next_state[board[ p.first][p.second]];
  }
  for (pos=0; pos<old_live_cells->size(); pos++){
      auto p = old_live_cells->at(pos);
      int i = p.first;
      int j = p.second;
      int nb = newboard[i][j]& 0x10;
      //std::cout << " i="<<i<<" j="<<j<< " newboard is "<<(int) nb << "\n";
      //assert(newboard[i][j] < 32);
        if (board[i][j]& 0x10 != nb){
          new_live_cells->push_back(pair<int,int>{i,j});
          newboard[i][j] &= 0x0f;

          if (i>0){
            if (j>0){
            if (newboard[i-1][j-1]>>5){
              new_live_cells->push_back(pair<int,int>{i-1,j-1});
              newboard[i-1][j-1] &= 0x0f;
            }
            }
          if (newboard[i-1][j]>>5){
            new_live_cells->push_back(pair<int,int>{i-1,j});
            newboard[i-1][j] &= 0x0f;
          }
          if (j<size){
            if (newboard[i-1][j+1]>>5){
              new_live_cells->push_back(pair<int,int>{i-1,j+1});
              newboard[i-1][j+1] &= 0x0f;
            }
          }
          }
          if (j>0){
          if (newboard[i][j-1]>>5){
            new_live_cells->push_back(pair<int,int>{i,j-1});
            newboard[i][j-1] &= 0x0f;
          }
          }

          if (j<size){
          if (newboard[i][j+1]>>5){
            new_live_cells->push_back(pair<int,int>{i,j+1});
            newboard[i][j+1] &= 0x0f;
          }
          }

          if (i<size){
          if (newboard[i+1][j-1]>>5){
            new_live_cells->push_back(pair<int,int>{i+1,j-1});
            newboard[i+1][j-1] &= 0x0f;
          }
          if (j<size){
            if (newboard[i+1][j+1]>>5){
              new_live_cells->push_back(pair<int,int>{i+1,j+1});
              newboard[i+1][j+1] &= 0x0f;
            }
          }
        }
            if(nb){
              newboard[i-1][j-1] += 1;
              newboard[i-1][j] += 1;
              newboard[i-1][j+1] += 1;
              newboard[i][j-1] += 1;
              newboard[i][j+1] += 1;
              newboard[i+1][j-1] += 1;
              newboard[i+1][j] += 1;
              newboard[i+1][j+1] += 1;
            } else {
              newboard[i-1][j-1] -= 1;
              newboard[i-1][j] -= 1;
              newboard[i-1][j+1] -= 1;
              newboard[i][j-1] -= 1;
              newboard[i][j+1] -= 1;
              newboard[i+1][j-1] -= 1;
              newboard[i+1][j] -= 1;
              newboard[i+1][j+1] -= 1;
            }
            //std::cout << "increasing neighbours i="<<i<<" j="<<j<< " newboard is "<<(int) newboard[i][j] << "\n";
            //assert(a == 1 || a == -1);
        }
    }
  //copy_borders(newboard, size);
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

void fill_board(cell_t ** board, int size, int percentage_alive) {
  int	i, j;
  srand(time(NULL));
  for (j=1; j<size+1; j++) {
    for (i=1; i<size+1; i++){
	    board[i][j] = ((int)(rand()) %percentage_alive == 0? 1 : 0 )<< 4;
    }
  }
  for (j=1; j<size+1; j++) {
    for (i=1; i<size+1; i++){
      if (board[i][j] >> 4){
          int a = 1;
          board[i-1][j-1] += a;
          board[i-1][j] += a;
          board[i-1][j+1] += a;
          board[i][j-1] += a;
          board[i][j+1] += a;
          board[i+1][j-1] += a;
          board[i+1][j] += a;
          board[i+1][j+1] += a;
      }
    }
  }

  for (i=1; i<size+1; i++){
    for (j=1; j<size+1; j++) {
      //std::cout << " i="<<i<<" j="<<j<< " value is "<<(int) board[i][j] << "\n";
	    assert((int)(board[i][j] & 0xf) == adjacent_to(board, i, j));
    }
  }
  //copy_borders(board, size);
}

void init_states_table(){
  for (int i =0; i<9; ++i){
     if (i == 2) {
       next_state[i] = i +32;
       next_state[i+16] = i+16 +32;
       next_state[i+32] = i +32;
       next_state[i+16+32] = i+16 +32;
     } else if (i == 3){
       next_state[i] = i+16 +32;
       next_state[i+16] = i+16 +32;
       next_state[i+32] = i +16+32;
       next_state[i+16+32] = i+16 +32;
     } else {
       next_state[i] = i +32;
       next_state[i+16] = i +32;
       next_state[i+32] = i +32;
       next_state[i+16+32] = i+32;
     }
  }
}

int main(int argc, char**argv){

  int size = std::atoi(argv[1]);
  int steps = std::atoi(argv[2]);
  int dead = std::atoi(argv[3]);
  cell_t ** prev = allocate_board (size+2);
  fill_board(prev, size, dead);
  cell_t ** next = allocate_board (size+2);
  cell_t ** tmp;
  int i;
  //print(prev,size);
  auto old_live_cells = new structures::ArrayList<std::pair<int,int>>{(size_t)size*size};
  auto new_live_cells = new structures::ArrayList<std::pair<int,int>>{(size_t)size*size};
  structures::ArrayList<std::pair<int,int>>* temp;
  for (int i = 1; i<size+1; ++i){
    for (int j = 1; j<size+1; ++j){
      old_live_cells->push_back(std::pair<int,int>{i,j});
    }
  }

  init_states_table();
  bool running = true;

  for (i=0; i<steps&&running; i++) {
        play (prev,next,size,  old_live_cells, new_live_cells);
        tmp = next;
        next = prev;
        prev = tmp;
        temp = old_live_cells;
        old_live_cells = new_live_cells;
        new_live_cells = temp;
    }

  //free_board(prev,size);
  //free_board(next,size);
        // splat down some random pixels


}
