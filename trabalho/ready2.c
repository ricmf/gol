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
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
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

cell_t ** board;
cell_t ** newboard;
sem_t *producer_sem1;
sem_t *producer_sem2;
sem_t *consumer_sem;

void *play(void *args) {
  int pos, i, j, a;
  int thread_number = *((int *) args);
  int size = ((int*)args)[1];
  int steps = ((int*)args)[2];
  int nthreads = ((int*)args)[3];
  cell_t** b = board;
  cell_t** nb = newboard;
  sem_t *temp = producer_sem1;
  /* for each cell, apply the rules of Life */
  int maxi = (int)(size/(double)nthreads*(thread_number+1));
  int mini = (int)(size/(double)nthreads*thread_number);
  //printf("start: %d finish: %d size: %d\n", mini, maxi, size);
  for (int k=0; k<steps; k++) {
    sem_wait(temp);
    for (i = mini+1; i<maxi+1; ++i) {
      for (j= 1; j<size+1; j++) {
        a = adjacent_to (b, size, i, j);
        if (a == 2) {
          nb[i][j] = b[i][j];
        } else if (a == 3) {
          nb[i][j] = 1 ;
        } else {
          nb[i][j] = 0;
        }
      }
    }
    if (temp == producer_sem2){
      temp = producer_sem1;
    } else {
      temp = producer_sem2;
    }
    b = board;
    nb = newboard;
    sem_post(consumer_sem);
  }
}

/* print the life board */
void print (cell_t ** board, int size) {
  int	i, j;
  /* for each row */
  for (j=1; j<size+1; j++) {
    /* print each column position... */
    for (i=1; i<size+1; i++)
      if(board[i][j]){
          printf ("■ ");
      } else {
        printf ("  ");
      }
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

void fill_board(cell_t ** board, int size, int percentage_alive) {
  int	i, j;
  srand(time(NULL));
  for (j=1; j<size+1; j++) {
    for (i=1; i<size+1; i++){
	    board[i][j] = ((int)(rand()) %percentage_alive == 0? 1 : 0 );
    }
  }
}

int main(int argc, char**argv){
int size, steps;
  if (argc<2 || atoi(argv[1])){
    FILE    *f;
    f = stdin;
    fscanf(f,"%d %d", &size, &steps);
    board = allocate_board (size+2);
    read_file (f, board,size);
    fclose(f);
    newboard = allocate_board (size+2);
  } else {
    steps = 1000;
    size = 100;
    board = allocate_board (size+2);
    newboard = allocate_board (size+2);
    fill_board(board, size, 32);
  }
  /**/
  sem_t p_sem1;
  sem_t p_sem2;
  sem_t* prod_sem = &p_sem2;
  sem_t c_sem;
  cell_t ** tmp;
  int i;
  //#ifdef DEBUG
  //printf("Initial:\n");
  //print(board,size);
  //#endif
  int nthreads = 4;
  pthread_t* threads= malloc(nthreads*sizeof(pthread_t));
  sem_init(&p_sem1, 0, nthreads);
  sem_init(&p_sem2, 0, 0);
  sem_init(&c_sem, 0, 0);
  producer_sem1 = &p_sem1;
  producer_sem2 = &p_sem2;
  consumer_sem = &c_sem;
  for (int j = 0; j<nthreads; j++){
    int *i = malloc(sizeof(*i)*4);
    i[0] = j;
    i[1] = size;
    i[2] = steps;
    i[3] = nthreads;
    pthread_create(&threads[j], NULL, play, (void *)i);
  }
  for (i=0; i<steps; i++) {
    for (int j = 0; j<nthreads; j++){
      sem_wait(consumer_sem);
    }
    #ifdef DEBUG
    //printf("%d ----------\n", i + 1);
    //print (newboard,size);
    #endif
    tmp = newboard;
    newboard = board;
    board = tmp;
    for (int j = 0; j<nthreads; j++){
      sem_post(prod_sem);
    }
    if (prod_sem == producer_sem2){
      prod_sem = producer_sem1;
    } else {
      prod_sem = producer_sem2;
    }
  }

  #ifdef RESULT
  //printf("Final:\n");
  //print (board,size);
  #endif

  free_board(newboard,size);
  free_board(board,size);
}
