#include <stdio.h>
#include <stdlib.h>
int foo(){
  return 0;
}



int main(){
    int j = 1;
    int* k = malloc(10000*4);
    #pragma omp parallel for
    for (long i = 0 ; i<100000000; ++i){
        j += k[i%10000];;
    }
    printf("%d\n",j );
}
