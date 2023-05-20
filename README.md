# parallelSortMPI
Sorting algorithms implement in serial and parallel MPI versions with OpenMP.

Sorting algorithms covered: Merge Sort, Quick Sort, and Enumeration Sort.

The main file to run everything is in mpiMain.c.

In addition to the sorting algorithms, the program also runs IO operations in serial and parallel.

## To compile:
Simply type `make` in the terminal. The compiled out file is set to be
“main”

## To run:
To run project 2, type:

  `make {run option} process={int value here} thread={int value here} size={int value here} action={string value here}`
  
**NOTE**: You must type make and {run option}. The others value are optional and if
empty will default to default values
  + run option: determine how the executable program will run
    * One of 4 options:
      * run: to run the general program of IO operations and sorting algo with the specify parameters below
      * reportIO: to run only IO operations in loop of 10 times and display the average time each and every operations
      * report: to run only the Sorting algos in loop of 10 times with the specify parameters below
      * runsmall: to run the general program like "run" but with size=10001 and action=all
  + process: determine the number of process for mpiexec run (default : 4)
  + thread: determine max number of omp thread each process will run (default : 2)
  + size: determine the array size to be generated and sorted (default : 10000001)
  + action: determine which sorting algorithm will run: (default : default)
    * One of 4 options, all will run serial and mpi solutions for each sort:
      - “default” for merge sort and quick sort run
      - “merge” for merge sort only run
      - “quick” for quick sort only run
      - “enum” for enumeration sort only run
      - “all” for all sort algorithms run

## Requirements:
- Environment: Ubuntu 20.04 or above
- Packages required: openmpi-bin openmpi-common openmpi-doc libopenmpi-dev
- To install packages: 
```bash
sudo apt-get install openmpi-bin openmpi-common openmpi-doc libopenmpi-dev
```
