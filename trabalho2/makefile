CC=mpicc

FLAGS=-O3 -Wno-unused-result
DEBUG=-DDEBUG
RESULT=-DRESULT

gol: t2.c
	$(CC) $(RESULT) $(FLAGS) t2.c -o t2exe

debug:
	$(CC) $(RESULT) $(DEBUG) $(FLAGS) t2.c -o t2exe

run:
	mpirun -np 3 ./t2exe < input-big.in

clean:
	rm -rf t2exe
