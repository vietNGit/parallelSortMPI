#include "mpiSorting.h"

#include "mpi.h"
#include <stdlib.h>
#include <stdio.h>

#include "extraFunc.h"
#include "quickSort.h"
#include "mergeSort.h"
#include "enumSort.h"

double *mpiSort(double *arr, int arrLen, int *procSizeAr, char *action){
    // Prepare each process for sorting of its own section
    int rank, parallelNum;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &parallelNum);

    MPI_Status status;
    
    int sortLen = procSizeAr[rank];
    int startAt = rank * procSizeAr[0];

    // Create sub array for each process
    // reAr is the return array
    double *subAr = copyFrom(arr, sortLen, startAt);

    // CALL THE CORRESPONDING SORTING ALGO BASE ON action
    // Make sure to call omp parallel version
    if (action[0] == 'q')
    {
        // Quick sorting algo
        subAr = paraQSHandler(subAr, sortLen);
    }
    else if (action[0] == 'm')
    {
        // Merge sorting algo
        subAr = paraMSHandler(subAr, sortLen);
    }
    else if (action[0] == 'e')
    {
        // Enumeration sorting algo
        subAr = paraEnumSort(subAr, sortLen);
    }
    else
    {
        printf("Action: %s does not exist. Fatal exit!\n", action);
        exit(EXIT_FAILURE);
    }
    

    // Merge array to the left
    int currentLen = sortLen;
    for(int step = 1; step < parallelNum; step = 2 * step)
    {
        // Step represent the differents in rank of send and receive
        // IE: step = rankSend - rankRecv
        if (rank % (2 * step) != 0) {
            // If process is on the sending side
            // Always exist
            // Right side

            int sendTo = rank - step;

            // Send the array size first
            MPI_Send(&currentLen, 1, MPI_INT, sendTo, 0, MPI_COMM_WORLD);

            MPI_Send(subAr, currentLen, MPI_DOUBLE, sendTo, 0, MPI_COMM_WORLD);
            
            // Any process that sends no longer need to run 
            break;
        }

        if (rank + step < parallelNum) {
            // If process is on the receiving side
            // May not need, incase of odd group
            // Left side
            int recvFrom = rank + step;

            // Receiving the array length first
            int recvSize;
            MPI_Recv(&recvSize, 1, MPI_INT, recvFrom, 0, MPI_COMM_WORLD, &status);

            double *recvAr = malloc(recvSize * sizeof(double));
            MPI_Recv(recvAr, recvSize, MPI_DOUBLE, recvFrom, 0, MPI_COMM_WORLD, &status);

            // Merge array just received with current array
            double *tempAr = mergeArray(subAr, recvAr, currentLen, recvSize);

            currentLen += recvSize;
            
            free(subAr);

            subAr = malloc(currentLen * sizeof(double));
            subAr = tempAr;
            // printArray(subAr, currentLen);
        }
    }

    // Finished merging every array
    // Only root process has the full sorted array
    MPI_Barrier(MPI_COMM_WORLD);

    return subAr;
}