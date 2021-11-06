#include "extraFunc.h"
#include "mergeSort.h"
#include <omp.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

double *paraMerging(double *arrA, double *arrB, int lenA, int lenB){
    // Loop through both array, compare accordingly and add to total array
    int sumLen = lenA + lenB;
    int dividePoint = sumLen / 2;
    double *totalAr = malloc(sumLen * sizeof(double));

    int lowA = 0;
    int lowB = 0;
    int highA = lenA - 1;
    int highB = lenB - 1;

    #pragma omp task
    {
        // Merging from left
        for (int i = 0; i < dividePoint; i++)
        {
            // Check for out of bound
            if(lowA >= lenA){
                // A out of bound
                totalAr[i] = arrB[lowB];
                lowB++;
            }
            else if (lowB >= lenB){
                // B out of bound
                totalAr[i] = arrA[lowA];
                lowA++;
            }else
            {
                // General cases
                if (arrA[lowA] <= arrB[lowB])
                {
                    // arrA element is smaller
                    totalAr[i] = arrA[lowA];
                    lowA++;
                }else
                {
                    // arrB element is smaller
                    totalAr[i] = arrB[lowB];
                    lowB++;
                }
            }
        }
    }

    #pragma omp task
    {
        // Merging from right
        for (int i = sumLen - 1; i >= dividePoint; i--)
        {
            // Check for out of bound
            if (highA < 0)
            {
                // A out of bound
                totalAr[i] = arrB[highB];
                highB--;    
            }
            else if (highB < 0)
            {
                // B out of bound
                totalAr[i] = arrA[highA];
                highA--;
            }
            else
            {
                // General case
                // Merging from the right means adding to array higher value element
                if (arrA[highA] <= arrB[highB])
                {
                    // arrB element is greater
                    totalAr[i] = arrB[highB];
                    highB--;
                }else
                {
                    // arrA element is greater
                    totalAr[i] = arrA[highA];
                    highA--;
                }
            }
        }
    }
    #pragma omp taskwait
     
     
    return totalAr;
}

double *mergeArray(double *arrA, double *arrB, int lenA, int lenB){    
    // printf("MERGING ARRAYS:\n");
    // printf("Array A: ");
    // printArray(arrA, lenA);
    // printf("Array B: ");
    // printArray(arrB, lenB);

    // Loop through both array, compare accordingly and add to total array
    int sumLen = lenA + lenB;
    double *totalAr = malloc(sumLen * sizeof(double));
    int indexA = 0;
    int indexB = 0;

    for (int i = 0; i < sumLen; i++){
        // Check for out of bound
        if(indexA >= lenA){
            // A out of bound
            totalAr[i] = arrB[indexB];
            indexB++;
        }
        else if (indexB >= lenB){
            // B out of bound
            totalAr[i] = arrA[indexA];
            indexA++;
        }else
        {
            // General cases
            if (arrA[indexA] <= arrB[indexB])
            {
                // arrA element is smaller
                totalAr[i] = arrA[indexA];
                indexA++;
            }else
            {
                // arrB element is smaller
                totalAr[i] = arrB[indexB];
                indexB++;
            }
        }
    }
     
    return totalAr;
}

double *mergeSort(double *arr, int arrLen){
    // Check if array can be divided
    if (arrLen < 2)
    {
        // Array of 1 element
        return arr;
    }
    else if (arrLen == 2)
    {
        // Array of 2 elements can be easily sorted to reduce recursion call
        if (arr[0] <= arr[1])
        {
            // Array already in order
            return arr;
        }
        else
        {
            double *returnAr = malloc(arrLen* sizeof(double));
            returnAr[0] = arr[1];
            returnAr[1] = arr[0];
            return returnAr;
        }
    }
    else if (arrLen == 3)
    {
        // Array ABC
        double valA = arr[0];
        double valB = arr[1];
        double valC = arr[2];
        
        // Determine A index
        if (valA < valB)
        {
            // A smaller than B
            // AB
            if (valB < valC)
            {
                // B smaller than C
                // BC
                // End array: ABC
                return arr;
            }
            else
            {
                // B greater than C
                // CB
                if (valA < valC)
                {
                    // A smaller than C
                    // AC
                    // End array: ACB
                    arr[1] = valC;
                    arr[2] = valB;
                    return arr;
                }
                else
                {
                    // A greater than C
                    // CA
                    // End array: CAB
                    arr[0] = valC;
                    arr[1] = valA;
                    arr[2] = valB;

                    return arr;
                }
                

            }
            

        }
        else
        {
            // A greater than B
            // BA
            if (valA < valC)
            {
                // A smaller than C
                // AC
                // End array: BAC
                arr[0] = valB;
                arr[1] = valA;
                return arr;
            }
            else
            {
                // A greater than C
                // CA
                if (valB < valC)
                {
                    // B smaller than C
                    // BC
                    // End array: BCA
                    arr[0] = valB;
                    arr[1] = valC;
                    arr[2] = valA;
                    return arr;
                }
                else
                {
                    // B greater than C
                    // CB
                    // End array: CBA
                    arr[0] = valC;
                    arr[2] = valA;
                    return arr;
                }                
            }
        }        
    }
    
    
    
    // GENERAL CASES
    // Divide array into 2 parts down the middle
    int dividePoint = arrLen / 2;
    int lenA = dividePoint;
    int lenB = arrLen - lenA;

    // Create 2 sub array from the original array
    // Total len of 2 array should be equal to arrlen
    // Array A copies from 0 to dividePoint - 1
    // Array B copies from dividePoint to end of arr
    double *arrA = copyArray(arr, lenA);
    double *arrB = copyFrom(arr, lenB, dividePoint);

    // Recursive call merge sort func
    double *sortedA = mergeSort(arrA, lenA);
    double *sortedB = mergeSort(arrB, lenB);

    // Merge the 2 sorted array
    double *sortedTotal = mergeArray(sortedA, sortedB, lenA, lenB);

    return sortedTotal;    
}

int buMerging(double *arr, int startIA, int startIB, int endI)
{
    // Create 2 array represent arrA and arrB
    int lenA = startIB - startIA;
    int lenB = endI - startIB;
    double *arrA = copyFrom(arr, lenA, startIA);
    double *arrB = copyFrom(arr, lenB, startIB);

    // printf("Important: %d; %d; %d\n", startIA, startIB, endI);
    // printf("Array A:\n");
    // printArray(arrA, lenA);
    // printf("Array B:\n");
    // printArray(arrB, lenB);
    

    // Compare and merge like usual to the original array
    int indexA = 0;
    int indexB = 0;
    for (int i = startIA; i < endI; i++)
    {
        // Check for out of bound
        if(indexA >= lenA){
            // A out of bound
            arr[i] = arrB[indexB];
            indexB++;
        }
        else if (indexB >= lenB){
            // B out of bound
            arr[i] = arrA[indexA];
            indexA++;
        }else
        {
            // General cases
            if (arrA[indexA] <= arrB[indexB])
            {
                // arrA element is smaller
                arr[i] = arrA[indexA];
                indexA++;
            }else
            {
                // arrB element is smaller
                arr[i] = arrB[indexB];
                indexB++;
            }
        }
    }

    free(arrA);
    free(arrB);

    return 0;
}

int buMergeSort(double *arr, int arrLen)
{
    // First loop through num values per merge group 2 groups: 1val each; 2 val each; 4 val each etc
    for (int numValsEach = 1; numValsEach < arrLen; numValsEach = 2 * numValsEach)
    {
        // printf("New width: %d \n", numValsEach);
        // printArray(arr, arrLen);
        // Second: Keep passing merge groups of established numValsEach from above to end of orgAr len
        // Group established by starting index 1, starting index 2(both inclusive), and ending index (exclusive): i, i + numValsEach and (i + 2 * numValsEach)
        // If ending index >= arrLen - 1: Use the arrLen - 1 as ending index
        for (int index = 0; index < arrLen; index = index + (2 * numValsEach))
        {
            int startIA = index;
            int startIB = index + numValsEach;
            if (startIB >= arrLen)
            {
                continue;
            }
            
            int endI = index + (2 * numValsEach);
            if (endI >= arrLen)
            {
                endI = arrLen;
            }

            // Pass through to merging array
            buMerging(arr, startIA, startIB, endI);            
        }
        
    }
    
    return 0;
}

double *msHandler(double *arr, int arrLen)
{    
    // double *returnAr = mergeSort(arr, arrLen);
    // return returnAr;

    // Bottom up merge sort
    // Copy array for sorting
    double *copAr = copyArray(arr, arrLen);

    buMergeSort(copAr, arrLen);

    return copAr;
}

double *paraMergeSort(double *arr, int arrLen){
    // Check if array can be divided
    if (arrLen < 2)
    {
        // Array of 1 element
        return arr;
    }
    else if (arrLen == 2)
    {
        // Array of 2 elements can be easily sorted to reduce recursion call
        if (arr[0] <= arr[1])
        {
            // Array already in order
            return arr;
        }
        else
        {
            double *returnAr = malloc(arrLen* sizeof(double));
            returnAr[0] = arr[1];
            returnAr[1] = arr[0];
            return returnAr;
        }
    }
    
    // GENERAL CASES
    // Divide array into 2 parts down the middle
    int dividePoint = arrLen / 2;
    int lenA = dividePoint;
    int lenB = arrLen - lenA;

    // Create 2 sub array from the original array
    // Total len of 2 array should be equal to arrlen
    // Array A copies from 0 to dividePoint - 1
    // Array B copies from dividePoint to end of arr
    // double *arrA = copyArray(arr, lenA);
    // double *arrB = copyFrom(arr, lenB, dividePoint);
    // printArray(arrA, lenA);

    // Recursive call merge sort func
    double *sortedA =  malloc(lenA* sizeof(double));;
    double *sortedB = malloc(lenB* sizeof(double));;

    // double *sortedA;
    // double *sortedB;

    #pragma omp task
    {
        double *arrA = copyArray(arr, lenA);
        // double *returnAr = paraMergeSort(arrA, lenA);
        // sortedA = (double *)returnAr;
        memcpy(sortedA, paraMergeSort(arrA, lenA), lenA* sizeof(double));
        // sortedA =  paraMergeSort(arrA, lenA);
        free(arrA);
        #pragma omp flush
    }
    #pragma omp task
    {
        double *arrB = copyFrom(arr, lenB, dividePoint);
        // double *returnAr = paraMergeSort(arrB, lenB);
        // sortedB = (double *)returnAr;
        memcpy(sortedB, paraMergeSort(arrB, lenB), lenB* sizeof(double));
        // sortedB = paraMergeSort(arrB, lenB);
        free(arrB);
        #pragma omp flush
    }
    #pragma omp taskwait

    // Merge the 2 sorted array
    double *sortedTotal = mergeArray(sortedA, sortedB, lenA, lenB);

    return sortedTotal;
}

int paraBUMSort(double *arr, int arrLen)
{
    // First loop through num values per merge group 2 groups: 1val each; 2 val each; 4 val each etc
    for (int numValsEach = 1; numValsEach < arrLen; numValsEach = 2 * numValsEach)
    {
        // printf("New width: %d \n", numValsEach);
        // printArray(arr, arrLen);
        // Second: Keep passing merge groups of established numValsEach from above to end of orgAr len
        // Group established by starting index 1, starting index 2(both inclusive), and ending index (exclusive): i, i + numValsEach and (i + 2 * numValsEach)
        // If ending index >= arrLen - 1: Use the arrLen - 1 as ending index
        #pragma omp parallel
        #pragma omp for
        for (int index = 0; index < arrLen; index = index + (2 * numValsEach))
        {
            int startIA = index;
            int startIB = index + numValsEach;
            
            if (startIB >= arrLen)
            {
                continue;
            }
            
            if (numValsEach == 1)
            {
                // Array of 2 elements can be easily sorted to reduce task call
                if (arr[startIA] <= arr[startIB])
                {
                    // Array already in order
                    continue;
                }
                else
                {
                    double temp = arr[startIB];
                    arr[startIB] = arr[startIA];
                    arr[startIA] = temp;
                    
                    continue;
                }
            }

            int endI = index + (2 * numValsEach);
            if (endI >= arrLen)
            {
                endI = arrLen;
            }

            // Pass through to merging array
            // #pragma omp task
            // {
                buMerging(arr, startIA, startIB, endI);            
            // }
        }
        // #pragma omp taskwait
    }
    
    return 0;
}

double *paraMSHandler(double *arr, int arrLen)
{
    // Bottom up merge sort
    // Copy array for sorting
    double *copAr = copyArray(arr, arrLen);

    // #pragma omp parallel
    //     #pragma omp single nowait
            paraBUMSort(copAr, arrLen);

    return copAr;
}