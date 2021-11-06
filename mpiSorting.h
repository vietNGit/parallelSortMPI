#ifndef MPISORTING_H
#define MPISORTING_H

/*
This function takes in unsorted array,
Delegate the work to different process and sort them.
Afterwards, merging of different processes sub array by step basis(Difficult to explain)
Parameters: unsorted array, array length, each process sub array size, sorting action to perform
Returns: sorted array in root process ONLY
*/
double *mpiSort(double *arr, int arrLen, int *procSizeAr, char *action);

#endif