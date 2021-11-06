CC = mpicc
CFLAGS = -std=c99 -Wall -Werror -pedantic -fopenmp
OBJ = mpiMain.c quickSort.c mergeSort.c enumSort.c extraFunc.c ioWork.c mpiSorting.c
TOCLEAN = main *.bin

.PHONY: project2make run reportIO report runsmall help clean

.DEFAULT_GOAL := project2make

project2make: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o main

process?=4
thread?=2
size?=10000001
action?="default"
run: main
	mpiexec -n $(process) main run $(thread) $(size) $(action)

reportIO: main
	mpiexec -n $(process) main reportIO $(thread) $(size) $(action)

report: main
	mpiexec -n $(process) main report $(thread) $(size) $(action)

runsmall: main
	mpiexec -n $(process) main run $(thread) 10001 all

define ANNOUNCE_HELP
- To compile the necessary file, type make.
- To run project 2 - option 1, type: 

make {run option} process={int value here} thread={int value here} size={int value here} action={string value here}

	+ run option: determine how the executable program will run
		* One of 4 options:
			run: to run the general program of IO operations and sorting algo with the specify parameters below
			reportIO: to run only IO operations in loop of 10 times and display the average time each and every operations
			report: to run only the Sorting algos in loop of 10 times with the specify parameters below
			runsmall: to run the general program like "run" but with size=10001 and action=all
	+ process: determine the number of process for mpiexec run (default : 4)
	+ thread: determine max number of omp thread each process will run (default : 2)
	+ size: determine the array size to be generated and sorted (default : 10000001)
	+ action: determine which sorting algorithm will run: (default : default)
		* One of 4 options, all will run serial and mpi solutions for each sort:
			“default” for merge sort and quick sort run
			“merge” for merge sort only run
			“quick” for quick sort only run
			“enum” for enumeration sort only run
			“all” for all sort algorithms run
endef

export ANNOUNCE_HELP

help:
	@echo "$$ANNOUNCE_HELP"

clean:
	rm -f $(TOCLEAN)