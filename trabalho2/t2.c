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
#include <time.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <mpi.h>

typedef unsigned char cell_t;

/* ------------------------------------------------------------------------- */

cell_t ** allocate_board (int size) {
  cell_t ** board = (cell_t **) (malloc(sizeof(cell_t*)*size+2))+1;
  int   i;
  for (i=-1; i<size+1; i++)
    board[i] = (cell_t *) (malloc(sizeof(cell_t)*size+2))+1;
  return board;
}

/* ------------------------------------------------------------------------- */

void free_board (cell_t ** board, int size) {
  int     i;
  for (i=0; i<size; i++)
    free(board[i]);
  free(board);
}

/* ------------------------------------------------------------------------- */

/* return the number of on cells adjacent to the i,j cell */
 inline int adjacent_to(cell_t ** board, int i, int j) {
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

/* ------------------------------------------------------------------------- */

/* print the life board */
void print (cell_t ** board, int size) {
  int   i, j;
  /* for each row */
  for (j=0; j<size; j++) {
    /* print each column position... */
    for (i=0; i<size; i++)
      if(board[i][j]){
          printf ("■ ");
      } else {
        printf ("  ");
      }
    /* followed by a carriage return */
    printf ("\n");
  }
}

/* ------------------------------------------------------------------------- */

/* read a file into the life board */
void read_file (FILE * f, cell_t ** board, int size) {
  int   i, j;
  char  *s = (char *) malloc(size+10);

  /* read the first new line (it will be ignored) */
  fgets (s, size+10,f);
  /* read the life board */
  for (j=0; j<size; j++) {
    /* get a string */
    fgets (s, size+10,f);
    /* copy the string to the life board */
    for (i=0; i<size; i++)
    board[i][j] = s[i] == 'x';
  }

  for (j=-1; j<size+1; j++) {
    board[-1][j] = 0;
    board[j][size] = 0;
    board[size][j] = 0;
    board[j][-1] = 0;
  }

}

 inline void get_next_state(cell_t** board, cell_t** newboard, int i, int j){
  int a = adjacent_to(board, i, j);
  if (a == 2) {
    newboard[i][j] = board[i][j];
  } else if (a == 3) {
    newboard[i][j] = 1 ;
  } else {
    newboard[i][j] = 0;
  }
}

/* ------------------------------------------------------------------------- */

int main(int argc, char**argv){

  MPI_Init(&argc, &argv);
  int size, steps;
  cell_t ** board;
  cell_t ** newboard;

    int world_size;
    MPI_Comm_size( MPI_COMM_WORLD, &world_size);
    int rank;
    MPI_Comm_rank( MPI_COMM_WORLD, &rank);
  if (rank ==0){

      FILE    *f;
      f = stdin;
      fscanf(f,"%d %d", &size, &steps);
      board = allocate_board (size);
      read_file (f, board, size);
      fclose(f);
      newboard = allocate_board (size);
  }
  cell_t ** tmp;
  int i;
  #ifdef DEBUG
    printf("Initial:\n");
    //print(board,size);
  #endif

  //como o  rank 0 é o processo principal, os ranks começam do 1 o process_number é o rank-1
  int process_number = rank-1;
  world_size--;
  //se for o processo inicial, não faz os calculos, apenas aguarda finalizarem
  MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&steps, 1, MPI_INT, 0, MPI_COMM_WORLD);

  if (rank > 0){
    board = allocate_board (size);
    newboard = allocate_board (size);
  }
  //manda as linhas iniciais
  for (int i = 0; i<size; ++i) {
    MPI_Bcast(board[i], size, MPI_CHAR, 0, MPI_COMM_WORLD);
  }
  if (rank != 0){
      int i, j;
      /* for each cell, apply the rules of Life */
      int maxi = (int)(size/(double)world_size*(process_number+1));
      int mini = (int)(size/(double)world_size*process_number);
      //printf("Procesos escravo rank %d mini %d maxi%d:\n",rank, mini, maxi);
      for (int k=0; k<steps; k++) {
        //calcula o resto
        for (i = mini; i<maxi; ++i) {
          for (j= 0; j<size; j++) {
            int a = adjacent_to(board, i, j);
            if (a == 2) {
              newboard[i][j] = board[i][j];
            } else if (a == 3) {
              newboard[i][j] = 1 ;
            } else {
              newboard[i][j] = 0;
            }
          }
        }
        MPI_Status request;
        // se não for a primeira seção, manda a primeira linha para o proc anterior
        if (rank > 1) {
          MPI_Send(newboard[mini], size, MPI_CHAR, rank - 1, k, MPI_COMM_WORLD);
        }
        if (rank < world_size){
          MPI_Recv(newboard[maxi], size, MPI_CHAR, rank+1, k, MPI_COMM_WORLD, &request);
        }
        if (rank < world_size){
          MPI_Send(newboard[maxi-1], size, MPI_CHAR, rank + 1, k, MPI_COMM_WORLD);
        }
        if (rank > 1) {
          MPI_Recv(newboard[mini-1], size, MPI_CHAR, rank-1, k, MPI_COMM_WORLD, &request);
        }
        tmp = newboard;
        newboard = board;
        board = tmp;
      }
      //manda suas linhas para o processo principal
      for (int i = mini; i<maxi; ++i) {
        //usando i como tag para receber na ordem certa
        MPI_Send(board[i], size, MPI_CHAR, 0, i, MPI_COMM_WORLD);
      }
  } else {
    //recebe os valores finais dos processos
    for (int i = 0; i<size; ++i) {
      MPI_Recv(board[i], size, MPI_CHAR, MPI_ANY_SOURCE, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
      #ifdef RESULT
        printf("Final:\n");
        //print (board,size);
      #endif
  }
  MPI_Finalize();
}
