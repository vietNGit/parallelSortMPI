#include "extraFunc.h"
#include "enumSort.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <omp.h>

double *enumSort(double *arr, int arrLen){
    double *returnAr = malloc(arrLen * sizeof(double));

    for (int i = 0; i < arrLen; i++)
    {
        // Choosing element to be ranked
        int rank = 0;
        int dupe = 0;

        for (int j = 0; j < arrLen; j++)
        {
            if(i == j){
                continue;
            }

            // Compare the element above to every other elements for ranking
            if (arr[i] > arr[j])
            {
                rank++;
            }
            else if (arr[i] == arr[j])
            {
                dupe++;
            }
        }

        // Put values to its corresponding rank
        if (returnAr[rank] == arr[i])
        {
            // Already added
            continue;
        }
        
        for (int j = 0; j <= dupe; j++)
        {
            returnAr[rank + j] = arr[i];
        }
    }

    return returnAr;
}

double *paraEnumSort(double *arr, int arrLen){
    double *returnAr = malloc(arrLen * sizeof(double));
    #pragma omp parallel
    {
        #pragma omp for
        for (int i = 0; i < arrLen; i++)
        {
            // Choosing element to be ranked
            int rank = 0;
            int dupe = 0;

            for (int j = 0; j < arrLen; j++)
            {
                if(i == j){
                    continue;
                }

                // Compare the element above to every other elements for ranking
                if (arr[i] > arr[j])
                {
                    rank++;
                }
                else if (arr[i] == arr[j])
                {
                    dupe++;
                }
            }

            // Put values to its corresponding rank
            if (returnAr[rank] == arr[i])
            {
                // Already added
                continue;
            }
            
            for (int j = 0; j <= dupe; j++)
            {
                returnAr[rank + j] = arr[i];
            }
        }
    }
    return returnAr;
}