#ifndef IOWORK_H
#define IOWORK_H

// Inform file open unsuccessfully
void notExist(char *fName);

/*
Take input file name then read to array and compare it to the array from parameter.
Return: 0 on succesfull, 1 on failure
*/
int compareFromFile(double *arr, int arrLen, char *fName);

/*
Serial version of writing array to bin file
*/
int writeToBin(double *arr, int arrLen, char *fName);

/*
Serial version of reading to array from bin file
*/
int readFromBin(double *reArr, int arrLen, char *fName);

// Parallel version of writing array to bin file
int mpiWriteFile(int arrLen, double *ogArr, char *fName, int *procArSize, int *displacements);

// Parallel version of reading to array from bin file
int mpiReadFile(int arrLen, double *reArr, char *fName, int *procArSize, int *displacements);

/*
IO run involves the generation of array;
Writing generated array to the file with serial method and mpi io operation;
Check the array writen to the file and read from the file are correct against the original array;
Also, check the time of read write between serial and mpi.
Parameters: array length, each process sub array size
Return: original generated array
*/
double *ioRun(int arrLen, int *procArSize, int *displacements);

/*
IO run involves the generation of array;
Writing generated array to the file with serial method and mpi io operation;
Check the array writen to the file and read from the file are correct against the original array;
Generate an array of time for each operation and return it. 
*/ 
double *ioReportRun(int arrLen, int *procArSize, int *displacements);

#endif