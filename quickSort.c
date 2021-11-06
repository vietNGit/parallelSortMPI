#include "extraFunc.h"
#include "quickSort.h"
#include <omp.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int quickSorting(double *arr, int arrLen, int lowIndex, int highIndex) {
    int pivot = lowIndex;

    // printf("quickSorting \n");
    if(!(lowIndex >= 0 && lowIndex < arrLen && highIndex >= 0 && highIndex < arrLen && lowIndex <= highIndex)){
        printf("error here\n");
        printf("%d : %d\n", lowIndex, highIndex);
        return 0;
    }
    // Loop through array and move elements according to pivot value
    for (int i=lowIndex + 1; i<=highIndex; i++){
        if (arr[i] <= arr[pivot])
        {
            // Swap with pivot value
            if(i == (pivot + 1)){
                // Perform 2 value swap
                double temp = arr[i];
                arr[i] = arr[pivot];
                arr[pivot] = temp;
                pivot++;
            }else{
                // Perform 3 value swap
                double temp = arr[i];
                arr[i] = arr[pivot + 1];
                arr[pivot + 1] = arr[pivot];
                arr[pivot] = temp;
                pivot++;
            }
        }
        
    }

    // Decide 2 new pivot for sorting
    if(pivot > lowIndex){        
        // Recursive call quickSorting func
        // Left partition
        quickSorting(arr, arrLen, lowIndex, pivot - 1);
    }
    if(pivot < highIndex){        
        // Recursive call quickSorting func
        // Right partition
        quickSorting(arr, arrLen, pivot + 1, highIndex);
    }

    return 0;
}

double *quickSort(double *arr, int arrLen){
    // Copy array for sorting
    double *copAr = copyArray(arr, arrLen);

    // printf("Copy array:\n");
    // for (int i=0; i < arrLen; i++){
    //     printf("%lf, ", arr[i]);
    // } 
    // printf("\n\n");

    // Sorting work
    // Choosing pivot indices, first element
    int lowBound = 0;
    int highBound = arrLen - 1;

    quickSorting(copAr, arrLen, lowBound, highBound);
    return copAr;
}

int paraQuickSort(double *arr, int arrLen, int lowIndex, int highIndex) {
    int pivot = lowIndex;

    // MOVING THE PRIVOT INTO CORRESPONDING INDEX, AFTERWARDS WILL BE USED AS NEW BOUND DIVIDER
    // printf("quickSorting \n");
    if(!(lowIndex >= 0 && lowIndex < arrLen && highIndex >= 0 && highIndex < arrLen && lowIndex <= highIndex)){
        printf("error here\n");
        printf("%d : %d\n", lowIndex, highIndex);
        return 0;
    }
    // Loop through array and move elements according to pivot value
    for (int i=lowIndex + 1; i<=highIndex; i++){
        if (arr[i] <= arr[pivot])
        {
            // Swap with pivot value
            if(i == (pivot + 1)){
                // Perform 2 value swap
                double temp = arr[i];
                arr[i] = arr[pivot];
                arr[pivot] = temp;
                pivot++;
            }else{
                // Perform 3 value swap
                double temp = arr[i];
                arr[i] = arr[pivot + 1];
                arr[pivot + 1] = arr[pivot];
                arr[pivot] = temp;
                pivot++;
            }
        }
        
    }

    // Decide 2 new pivot for sorting
    #pragma omp task
    {
        if(pivot > lowIndex){        
            // Recursive call quickSorting func
            // Left partition
            paraQuickSort(arr, arrLen, lowIndex, pivot - 1);
        }
    }
    #pragma omp task
    {
        if(pivot < highIndex){        
            // Recursive call quickSorting func
            // Right partition
            paraQuickSort(arr, arrLen, pivot + 1, highIndex);
        }
    }
    
    return 0;
}

double *paraQSHandler(double *arr, int arrLen){
    // Copy array for sorting
    double *copAr = copyArray(arr, arrLen);

    // Sorting work
    // Choosing pivot indices, first element
    int lowBound = 0;
    int highBound = arrLen - 1;

    #pragma omp parallel
    {
        #pragma omp single
        {
            paraQuickSort(copAr, arrLen, lowBound, highBound);
        }
    }

    return copAr;
}