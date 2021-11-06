#ifndef EXTRAFUNC_H
#define EXTRAFUNC_H
/* ^^ these are the include guards */

// Function copy a sub section of the original array from the designated startPoint
double *copyFrom(double *orgAr, int arLen, int startPoint);

// Function copy original array from 0 to secify arLen
double  *copyArray(double *orgAr, int arLen);

// Generate a random array with length arrLen
void arrayGen(double *arr, int arrLen);

// Compare values in arr to model. Return 1 if mismatch
int arrayCheck(double *model, double *arr, int arrLen);

// Print contents of array for a sepcify length
void printArray(double *ar, int arrLen);

// Function to determine if string is numeric. Return 1 on success
int isInteger(char *str);

// Return smaller integer
int minimum(int a, int b);

/*
Takes in former and latter value.
Calculate the percentage reduction going from former value to latter value.
If return value is possitive, latter is smaller than former.
If return value is negative, latter is greater than former.
*/
double percentReduct(double former, double latter);

// Fuction returns a list of corresponding array length divided for each process
int *processArrSize(int arrLen);

// Function returns a list of corresponding sub array start index for each process
int *subArStartAt(int arrLen);

#endif