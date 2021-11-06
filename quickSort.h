#ifndef QUICKSORT_H
#define QUICKSORT_H

/*----------------------------------------------------------------
    Serialization quick sort algorithm implementation.
    Call quickSort instead of calling this function directly. 
*/
int quickSorting(double *arr, int arrLen, int lowIndex, int highIndex);

/*----------------------------------------------------------------
Function handler responsible for calling quick sorting function.
Use this instead of calling quickSorting directly.
*/
double *quickSort(double *arr, int arrLen);

/*----------------------------------------------------------------
Parallelization quick sort algorithm implementation.
Call paraQSHandler instead of calling this function directly. 
*/
int paraQuickSort(double *arr, int arrLen, int lowIndex, int highIndex);

/*----------------------------------------------------------------
Function handler responsible for calling parallelization quick sorting function.
Use this instead of calling paraQuickSort directly.
*/
double *paraQSHandler(double *arr, int arrLen);

#endif