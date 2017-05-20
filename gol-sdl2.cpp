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
  cell_t ** board = (cell_t **) malloc(sizeof(cell_t*)*size);
  int	i;
  for (i=0; i<size; i++)
  board[i] = (cell_t *) malloc(sizeof(cell_t)*size);
  return board;
}


int  next_state[64];

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
  , structures::ArrayList<std::pair<int,int>>* new_live_cells, vector<unsigned char>* pixels) {
  int	pos, i, j, a;
  /* for each cell, apply the rules of Life */
  new_live_cells->clear();
  #pragma omp parallel for private(j)
  for (i=1; i<size+1; i++){
    for (j=1; j<size+1; j++) {
      newboard[i][j] = next_state[board[i][j]];
      if (board[i][j] != newboard[i][j]){
         if (newboard[i][j] >> 4){
           const unsigned int offset = ( (size+2) * 4 * j ) + i * 4;
           pixels->at(offset ) = 255;        // b
           pixels->at(offset + 1) =  255;        // g
           pixels->at(offset + 2) =  255;        // r
           pixels->at(offset + 3) = SDL_ALPHA_OPAQUE;
         } else {
           const unsigned int offset = ( (size+2) * 4 * j ) + i * 4;
           pixels->at( offset) =  0;        // b
           pixels->at( offset + 1 ) =  0;        // g
           pixels->at( offset + 2 ) =  0;        // r
           pixels->at( offset + 3 ) = SDL_ALPHA_OPAQUE;
         }
      }
    }
  }
  #pragma omp barrier
  #pragma omp parallel for private(j)
  for (i=1; i<size+1; i++){
    for (j=1; j<size+1; j++) {
      int nb = newboard[i][j]>>4;
      //std::cout << " i="<<i<<" j="<<j<< " newboard is "<<(int) nb << "\n";
      //assert(newboard[i][j] < 32);
        if (board[i][j]>>4 != nb){
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
    }
  copy_borders(newboard, size);
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
  copy_borders(board, size);
}

void init_states_table(){
  for (int i =0; i<9; ++i){
     if (i == 2 ) {
       next_state[i] = i;
       next_state[i+16] = i+16;
     } else if (i == 3){
       next_state[i] = i+16;
       next_state[i+16] = i+16;
     } else {
       next_state[i] = i;
       next_state[i+16] = i;
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


  const unsigned int texWidth = size+2;
  const unsigned int texHeight = size+2;


  SDL_Init( SDL_INIT_EVERYTHING );
  atexit( SDL_Quit );

  SDL_Window* window = SDL_CreateWindow      (      "SDL2",      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      size>720?720:720, size>720?720:720,      SDL_WINDOW_SHOWN      );

  SDL_Renderer* renderer = SDL_CreateRenderer
      (
      window,
      -1,
      SDL_RENDERER_ACCELERATED
      );

  SDL_RendererInfo info;
  SDL_GetRendererInfo( renderer, &info );
  //cout << "Renderer name: " << info.name << endl;
  //cout << "Texture formats: " << endl;
  for( Uint32 i = 0; i < info.num_texture_formats; i++ )  {
      //cout << SDL_GetPixelFormatName( info.texture_formats[i] ) << endl;
  }
  SDL_Texture* texture = SDL_CreateTexture (      renderer,      SDL_PIXELFORMAT_ARGB8888,
          SDL_TEXTUREACCESS_STREAMING,      texWidth, texHeight      );



  SDL_Event event;

  init_states_table();
  bool running = true;

  vector< unsigned char > pixels( texWidth * texHeight * 4, 0 );

  for (i=0; i<steps&&running; i++) {
      const Uint64 start = SDL_GetPerformanceCounter();

      SDL_SetRenderDrawColor( renderer, 0, 0, 0, SDL_ALPHA_OPAQUE );
      SDL_RenderClear( renderer );

      while( SDL_PollEvent( &event ) )      {
          if( ( SDL_QUIT == event.type ) ||
              ( SDL_KEYDOWN == event.type && SDL_SCANCODE_ESCAPE == event.key.keysym.scancode ) )          {
              running = false;
              break;
          }
      }
      SDL_Delay(100);

        play (prev,next,size,  old_live_cells, new_live_cells, &pixels);
        tmp = next;
        next = prev;
        prev = tmp;
        temp = old_live_cells;
        old_live_cells = new_live_cells;
        new_live_cells = temp;

      SDL_UpdateTexture(texture, NULL,&pixels[0],texWidth * 4);
      SDL_RenderCopy( renderer, texture, NULL, NULL );
      SDL_RenderPresent( renderer );

      const Uint64 end = SDL_GetPerformanceCounter();
      const static Uint64 freq = SDL_GetPerformanceFrequency();
      const double seconds = ( end - start ) / static_cast< double >( freq );
      //cout << "Frame time: " << seconds * 1000.0 << "ms" << endl;
  }

  free_board(prev,size);
  free_board(next,size);

  SDL_DestroyRenderer( renderer );
  SDL_DestroyWindow( window );
  SDL_Quit();
        // splat down some random pixels


}
