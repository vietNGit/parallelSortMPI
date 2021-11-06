#include "extraFunc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "mpi.h"

double *copyFrom(double *orgAr, int arLen, int startPoint){
    double *copAr = malloc(arLen* sizeof(double));
    // memcpy(copAr, orgAr + dividePoint, arLen*sizeof(double));
    memcpy(copAr, &orgAr[startPoint], arLen*sizeof(double));
    return copAr;
}

double  *copyArray(double *orgAr, int arLen){
    double *copAr = malloc(arLen* sizeof(double));
    memcpy(copAr, &orgAr[0], arLen* sizeof(double));
    return copAr;
}

void arrayGen(double *arr, int arrLen){
    int max_val = arrLen; 
    for (int i=0; i < arrLen; i++){
        arr[i] = ((double)rand()/(double)(RAND_MAX)) * max_val;
    }

}

int arrayCheck(double *model, double *arr, int arrLen){
    for (int i=0; i < arrLen; i++){
        if(model[i] != arr[i]){
            printf("\nMISMATCH---------------------\n");
            
            printf("Model: %lf vs : %lf at %d\n\n", model[i], arr[i], i);
            return 1;
        }
    }

    return 0;
}

void printArray(double *ar, int arrLen)
{
    printf("[");
    for (int i = 0; i < arrLen; i++)
    {
        printf("%lf", ar[i]);
        if (i < arrLen - 1)
        {
            printf(", ");
        }
    }
    printf("]\n");
}

int isInteger(char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (isdigit(str[i]) == 0)
        {   
            // Contain non numeric value
            return 0;
        }
    }

    return 1;
}

int minimum(int a, int b)
{
    if (a < b)
    {
        return a;
    }
    else
    {
        return b;
    }
}

double percentReduct(double former, double latter){
    return ((former - latter) / former) * 100;
}

int *processArrSize(int arrLen){
    int parallelNum;
    MPI_Comm_size(MPI_COMM_WORLD, &parallelNum);

    int *reAr = malloc(parallelNum * sizeof(int));
    int averageLen = arrLen / parallelNum;
    for (int i = 0; i < parallelNum; i++)
    {
        reAr[i] = averageLen;
    }

    reAr[parallelNum - 1] = arrLen - ((parallelNum - 1) * averageLen);

    return reAr;    
}

int *subArStartAt(int arrLen){
    int parallelNum;
    MPI_Comm_size(MPI_COMM_WORLD, &parallelNum);

    int *reAr = malloc(parallelNum * sizeof(int));
    int averageLen = arrLen / parallelNum;
    for (int i = 0; i < parallelNum; i++)
    {
        reAr[i] = i * averageLen;
    }

    return reAr;
}