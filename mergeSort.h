#ifndef MERGESORT_H
#define MERGESORT_H

/*----------------------------------------------------------------
    Parallelization array merging algorithm implementation.
*/
double *paraMerging(double *arrA, double *arrB, int lenA, int lenB);

/*----------------------------------------------------------------
    Serialization array merging algorithm implementation.
*/
double *mergeArray(double *arrA, double *arrB, int lenA, int lenB);

/*----------------------------------------------------------------
Serialization merge sort top-down algorithm implementation.
*/
double *mergeSort(double *arr, int arrLen);

/*----------------------------------------------------------------
    Serialization array merging algorithm implementation.
    This function is used in bottom-up merge sort function.
*/
int buMerging(double *arr, int startIA, int startIB, int endI);

/*----------------------------------------------------------------
Serialization merge sort bottom-up algorithm implementation.
*/
int buMergeSort(double *arr, int arrLen);

/*----------------------------------------------------------------
Function handler responsible for calling merge sorting function.
Use this instead of calling any merge sort function directly.
Default: call bottom-up merge sort implementation.
*/
double *msHandler(double *arr, int arrLen);

/*----------------------------------------------------------------
Parallelization merge sort top-down algorithm implementation.
*/
double *paraMergeSort(double *arr, int arrLen);

/*----------------------------------------------------------------
Parallelization merge sort bottom-up algorithm implementation.
*/
int paraBUMSort(double *arr, int arrLen);

/*----------------------------------------------------------------
Function handler responsible for calling parallelization merge sorting function.
Use this instead of calling any parallelization merge sort function directly.
Default: call bottom-up parallelization merge sort implementation.
*/
double *paraMSHandler(double *arr, int arrLen);

#endif