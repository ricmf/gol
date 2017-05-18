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


using namespace std;
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

inline void copy_borders(cell_t** board, int size){
  for (int i=0; i<size+1; i++){
    board[i][0] = board[i][size];
    board[i][size+1] = board[i][1];
    board[0][i] = board[size][i];
    board[size+1][i] = board[1][i];
  }
}

inline void play (cell_t ** board, cell_t ** newboard, int size, structures::ArrayList<std::pair<int,int>>* old_live_cells
  , structures::ArrayList<std::pair<int,int>>* new_live_cells, vector<unsigned char>* pixels) {
  int	pos, i, j, a;
  /* for each cell, apply the rules of Life */
  new_live_cells->clear();
  for (i=1; i<size+1; i++){
    for (j=1; j<size+1; j++) {
    a = adjacent_to (board, size, i, j);
    if (a == 2) newboard[i][j] = board[i][j];
    else if (a == 3) newboard[i][j] = 1;
    //else if (a < 2) newboard[i][j] = 0;
    else newboard[i][j] = 0;
    if (board[i][j] != newboard[i][j]){
       /*if (newboard[i][j]){
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
       }*/
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

void fill_board(cell_t ** board, int size) {
  int	i, j;
  srand(time(NULL));
  for (j=1; j<size+1; j++) {
    for (i=1; i<size+1; i++)
	    board[i][j] = (int)(rand()) %16 == 0? 1 : 0;
  }
  copy_borders(board, size);
}

int main(int argc, char**argv){

  int size = std::atoi(argv[1]);
  int steps = std::atoi(argv[2]);
  cell_t ** prev = allocate_board (size+2);
  fill_board(prev, size);
  cell_t ** next = allocate_board (size+2);
  cell_t ** tmp;
  int i;
  //print(prev,size);
  auto old_live_cells = new structures::ArrayList<std::pair<int,int>>{size*size};
  auto new_live_cells = new structures::ArrayList<std::pair<int,int>>{size*size};
  structures::ArrayList<std::pair<int,int>>* temp;
  for (int i = 1; i<size+1; ++i){
    for (int j = 1; j<size+1; ++j){
      old_live_cells->push_back(std::pair<int,int>{i,j});
    }
  }


  const unsigned int texWidth = size+2;
  const unsigned int texHeight = size+2;

  /*

  SDL_Init( SDL_INIT_EVERYTHING );
  atexit( SDL_Quit );

  SDL_Window* window = SDL_CreateWindow      (      "SDL2",      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      720, 720,      SDL_WINDOW_SHOWN      );

  SDL_Renderer* renderer = SDL_CreateRenderer
      (
      window,
      -1,
      SDL_RENDERER_ACCELERATED
      );

  SDL_RendererInfo info;
  SDL_GetRendererInfo( renderer, &info );
  cout << "Renderer name: " << info.name << endl;
  cout << "Texture formats: " << endl;
  for( Uint32 i = 0; i < info.num_texture_formats; i++ )  {
      cout << SDL_GetPixelFormatName( info.texture_formats[i] ) << endl;
  }
  SDL_Texture* texture = SDL_CreateTexture (      renderer,      SDL_PIXELFORMAT_ARGB8888,
          SDL_TEXTUREACCESS_STREAMING,      texWidth, texHeight      );



  SDL_Event event;
  */
  bool running = true;

  vector< unsigned char > pixels( texWidth * texHeight * 4, 0 );

  for (i=0; i<steps&&running; i++) {
      /*const Uint64 start = SDL_GetPerformanceCounter();

      SDL_SetRenderDrawColor( renderer, 0, 0, 0, SDL_ALPHA_OPAQUE );
      SDL_RenderClear( renderer );

      while( SDL_PollEvent( &event ) )      {
          if( ( SDL_QUIT == event.type ) ||
              ( SDL_KEYDOWN == event.type && SDL_SCANCODE_ESCAPE == event.key.keysym.scancode ) )          {
              running = false;
              break;
          }
      }*/

        play (prev,next,size,  old_live_cells, new_live_cells, &pixels);
        tmp = next;
        next = prev;
        prev = tmp;
        temp = old_live_cells;
        old_live_cells = new_live_cells;
        new_live_cells = temp;

      /*SDL_UpdateTexture(texture, NULL,&pixels[0],texWidth * 4);
      SDL_RenderCopy( renderer, texture, NULL, NULL );
      SDL_RenderPresent( renderer );

      const Uint64 end = SDL_GetPerformanceCounter();
      const static Uint64 freq = SDL_GetPerformanceFrequency();
      const double seconds = ( end - start ) / static_cast< double >( freq );*/
      //cout << "Frame time: " << seconds * 1000.0 << "ms" << endl;
  }

  free_board(prev,size);
  free_board(next,size);

  //SDL_DestroyRenderer( renderer );
  //SDL_DestroyWindow( window );
  //SDL_Quit();
        // splat down some random pixels


}
