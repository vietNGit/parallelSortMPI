#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>
#include <omp.h>

#include "mpi.h"
#include <unistd.h>

#include "quickSort.h"
#include "mergeSort.h"
#include "extraFunc.h"
#include "enumSort.h"
#include "ioWork.h"
#include "mpiSorting.h"

/*
Compare function for library sort
*/
int compare_function(const void *a,const void *b) {
    double *x = (double *) a;
    double *y = (double *) b;
    // return *x - *y; // this is WRONG...
    if (*x < *y) return -1;
    else if (*x > *y) return 1; 
    return 0;
}

/*
-----------------------------------------------------------------------------------
REPORT RUNNING SECTION STARTS HERE
Within this section is the report running code
Where a non print version of the normal run code is loop for specify time 
To generate the average run time of any operations or algorithms
This sections are not necessary to the project and can be disregards
*/

/*
Runs and compare different sorting algo: merge, quick, enum
Between 2 version serial, mpi-omp
Then record the time of each sorting run for return
Parameters: original array, array length
Return: time array for each sorting run
*/
double *sortReportRun(double *ogArr, int arrLen, char *action, int *procArSize){
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double *timeArray;
    if (action[0] == 'e' || action[0] == 'a')
    {
        timeArray = malloc(6 * sizeof(double));
    }
    else
    {
        timeArray = malloc(4 * sizeof(double));
    }

    // Sort the array with library funtion
    double *libSort = copyArray(ogArr, arrLen);
    struct timeval start, end;
    
    if (rank == 0)
    {
        gettimeofday(&start, NULL);
        qsort(libSort, arrLen, sizeof(double), compare_function);
        gettimeofday(&end, NULL);
    }
    
    MPI_Barrier(MPI_COMM_WORLD);

    // START SORTING
    // ACTION HANDLING
    // action specified what sorting algo to display
    // m for merge sort
    // q for quick sort
    // e for enum sort
    // d for default: no enum sort
    // a for all

    // MERGE SORT ACTION
    if(action[0] == 'a' || action[0] == 'm' || action[0] == 'd'){
        // Serial Merge sort check
        if (rank == 0)
        {
            gettimeofday(&start, NULL);
            double *serialMergeS = msHandler(ogArr, arrLen);
            gettimeofday(&end, NULL);

            timeArray[0] = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
    
            if (arrayCheck(libSort, serialMergeS, arrLen))
            {
                printf("Serial Merge sort algo broken\n\n");
                exit(EXIT_FAILURE);
            }

            free(serialMergeS);
        }
        
        MPI_Barrier(MPI_COMM_WORLD);

        // MPI Merge sort check
        gettimeofday(&start, NULL);
        double *mpiMergeSort = mpiSort(ogArr, arrLen, procArSize, "merge");
        gettimeofday(&end, NULL);

        timeArray[1] = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
        
        if (rank == 0)
        {
            // Array checking should only be done in root process
            if (arrayCheck(libSort, mpiMergeSort, arrLen))
            {
                printf("MPI Merge sort algo broken\n\n");
                exit(EXIT_FAILURE);
            }
        }

        free(mpiMergeSort);
    }

    // QUICK SORT ACTION
    if(action[0] == 'a' || action[0] == 'q' || action[0] == 'd'){
        // Serial quick sort check
        if (rank == 0)
        {
            gettimeofday(&start, NULL);
            double *serialQuickS = quickSort(ogArr, arrLen);
            gettimeofday(&end, NULL);

            timeArray[2] = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;

            if (arrayCheck(libSort, serialQuickS, arrLen))
            {
                printf("Serial Quick sort algo broken\n\n");
                exit(EXIT_FAILURE);
            }

            free(serialQuickS);
        }
        
        MPI_Barrier(MPI_COMM_WORLD);

        // MPI Quick sort check...
        gettimeofday(&start, NULL);
        double *mpiQuickS = mpiSort(ogArr, arrLen, procArSize, "quick");
        gettimeofday(&end, NULL);

        timeArray[3] = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;

        if (rank == 0)
        {
            // Array checking should only be done in root process
            if (arrayCheck(libSort, mpiQuickS, arrLen))
            {
                printf("MPI Quick sort algo broken\n\n");
                exit(EXIT_FAILURE);
            }
        }
        
        free(mpiQuickS);
    }

    // ENUMERATION SORT ACTION
    if(action[0] == 'a' || action[0] == 'e'){
        // Serial enum sort check
        if (rank == 0)
        {
            gettimeofday(&start, NULL);
            double *serialEnumS = enumSort(ogArr, arrLen);
            gettimeofday(&end, NULL);

            timeArray[4] = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;

            if (arrayCheck(libSort, serialEnumS, arrLen))
            {
                printf("Serial Enum sort algo broken\n\n");
                exit(EXIT_FAILURE);
            }

            free(serialEnumS);
        }
        
        MPI_Barrier(MPI_COMM_WORLD);

        // Enumeration sort check
        gettimeofday(&start, NULL);
        double *mpiEnumS = mpiSort(ogArr, arrLen, procArSize, "enum");
        gettimeofday(&end, NULL);

        timeArray[5] = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;

        if (rank == 0)
        {
            // Array checking should only be done in root process
            if (arrayCheck(libSort, mpiEnumS, arrLen))
            {
                printf("MPI Enum sort algo broken\n\n");
                exit(EXIT_FAILURE);
            }
        }
        
        free(mpiEnumS);
    }

    free(libSort);
    
    MPI_Barrier(MPI_COMM_WORLD);

    return timeArray;
}

int sortReport(int numThreads, int arrLen, char *action, int timesRun){
    omp_set_num_threads(numThreads);
    int rank, parallelNum;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &parallelNum);

    if (rank == 0)
    {
        printf("Running report for Sorting algorithms with MPI size: %d; omp num threads: %d ; array size: %d ; action: %s\n\n", parallelNum, omp_get_max_threads(), arrLen, action);
    }

    // Determine each process sub array size
    int *procArSize = processArrSize(arrLen);

    MPI_Barrier(MPI_COMM_WORLD);

    double *ogArr = malloc(arrLen * sizeof(double));
    if (rank == 0)
    {
        // Array generated
        arrayGen(ogArr, arrLen);
    }

    // Root sends generated array to everyone
    MPI_Bcast(ogArr, arrLen, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    // Start the loop for results 
    double timeMS = 0;
    double timeMPIMS = 0;
    
    double timeQS = 0;
    double timeMPIQS = 0;

    double timeES = 0;
    double timeMPIES = 0;

    for (int i = 0; i < timesRun; i++)
    {
        double *timeArray = sortReportRun(ogArr, arrLen, action, procArSize);

        timeMS += timeArray[0];
        timeMPIMS += timeArray[1];
        
        timeQS += timeArray[2];
        timeMPIQS += timeArray[3];

        if (action[0] == 'e' || action[0] == 'a')
        {
            timeES += timeArray[4];
            timeMPIES += timeArray[5];
        }
        
        free(timeArray);
    }
     
    if (rank == 0)
    {
        double avTimeMS = timeMS / (double)timesRun;
        double avTimeMPIMS = timeMPIMS / (double)timesRun;
        
        double avTimeQS = timeQS / (double)timesRun;  
        double avTimeMPIQS = timeMPIQS / (double)timesRun;

        printf("Average time for serial merge sort: %12.10f\n", avTimeMS);
        printf("Average time for MPI merge sort: %12.10f\n", avTimeMPIMS);
        printf("Average merge sort MPI time reduction (percent): %12.10f \n\n", percentReduct(avTimeMS, avTimeMPIMS));

        printf("Average time for serial quick sort: %12.10f\n", avTimeQS);
        printf("Average time for MPI quick sort: %12.10f\n", avTimeMPIQS);
        printf("Average quick sort MPI time reduction (percent): %12.10f \n\n", percentReduct(avTimeQS, avTimeMPIQS));
    
        if (action[0] == 'e' || action[0] == 'a')
        {
            double avTimeES = timeES / (double)timesRun;
            double avTimeMPIES = timeMPIES / (double)timesRun;

            printf("Average time for serial enum sort: %12.10f\n", avTimeES);
            printf("Average time for MPI enum sort: %12.10f\n", avTimeMPIES);
            printf("Average enum sort MPI time reduction (percent): %12.10f \n\n", percentReduct(avTimeES, avTimeMPIES));
        }
        
    }

    MPI_Barrier(MPI_COMM_WORLD);

    return 0;
}

int reportIO(int numThreads, int arrLen, char *action, int timesRun){
    omp_set_num_threads(numThreads);
    int rank, parallelNum;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &parallelNum);

    if (rank == 0)
    {
        printf("Running report for IO operations with MPI size: %d; omp num threads: %d ; array size: %d ; action: %s\n\n", parallelNum, omp_get_max_threads(), arrLen, action);
    }
    
    MPI_Barrier(MPI_COMM_WORLD);

    int *procArSize = processArrSize(arrLen);
    int *displacements = subArStartAt(arrLen);

    double timeSerialWrite = 0;
    double timeMPIWrite = 0;

    double timeSerialRead = 0;    
    double timeMPIRead = 0;

    for (int i = 0; i < timesRun; i++)
    {
        double *timeArray = ioReportRun(arrLen, procArSize, displacements);

        timeSerialWrite += timeArray[0];
        timeMPIWrite += timeArray[1];

        timeSerialRead += timeArray[2];    
        timeMPIRead += timeArray[3];

        free(timeArray);
    }
    
    if (rank == 0)
    {
        double avTimeSerialWrite = timeSerialWrite / (double)timesRun;
        double avTimeMPIWrite = timeMPIWrite / (double)timesRun;
        
        double avTimeSerialRead = timeSerialRead / (double)timesRun;  
        double avTimeMPIRead = timeMPIRead / (double)timesRun;

        printf("Average time for serial write: %12.10f\n", avTimeSerialWrite);
        printf("Average time for MPI write: %12.10f\n", avTimeMPIWrite);
        printf("Average Write IO MPI time reduction (percent): %12.10f \n\n", percentReduct(avTimeSerialWrite, avTimeMPIWrite));

        printf("Average time for serial read: %12.10f\n", avTimeSerialRead);
        printf("Average time for MPI read: %12.10f\n", avTimeMPIRead);
        printf("Average Read IO MPI time reduction (percent): %12.10f \n\n", percentReduct(avTimeSerialRead, avTimeMPIRead));
    }

    MPI_Barrier(MPI_COMM_WORLD);

    return 0;
}

/*
REPORT RUNNING SECTION ENDS HERE
Within this section is the report running code
Where a non print version of the normal run code is loop for specify time 
To generate the average run time of any operations or algorithms
This sections are not necessary to the project and can be disregards
---------------------------------------------------------------------
*/

/*
Runs and compare different sorting algo: merge, quick, enum
Between 2 version serial, mpi-omp
Parameters: original array, array length
*/
int sortRun(double *ogArr, int arrLen, char *action, int *procArSize){
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Sort the array with library funtion
    double *libSort = copyArray(ogArr, arrLen);
    struct timeval start, end;
    
    if (rank == 0)
    {
        gettimeofday(&start, NULL);
        qsort(libSort, arrLen, sizeof(double), compare_function);
        gettimeofday(&end, NULL);

        double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
        printf("Library sort time spent = %12.10f\n\n",delta);    
    }
    
    // MPI_Bcast(libSort, arrLen, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    // START SORTING
    // ACTION HANDLING
    // action specified what sorting algo to display
    // m for merge sort
    // q for quick sort
    // e for enum sort
    // d for default: no enum sort
    // a for all

    // MERGE SORT ACTION
    if(action[0] == 'a' || action[0] == 'm' || action[0] == 'd'){
        // Serial Merge sort check
        double serialMS = (double)0;
        if (rank == 0)
        {
            gettimeofday(&start, NULL);
            double *serialMergeS = msHandler(ogArr, arrLen);
            gettimeofday(&end, NULL);

            serialMS = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
            printf("Serial Merge sort time spent = %12.10f\n",serialMS);

            if (arrayCheck(libSort, serialMergeS, arrLen))
            {
                printf("Serial Merge sort algo broken\n\n");
                exit(EXIT_FAILURE);
            }
            else{
                printf("Serial Merge sort runs successfully\n\n");
            }

            free(serialMergeS);
        }
        
        MPI_Barrier(MPI_COMM_WORLD);

        // MPI Merge sort check
        gettimeofday(&start, NULL);
        double *mpiMergeSort = mpiSort(ogArr, arrLen, procArSize, "merge");
        gettimeofday(&end, NULL);

        double mpiMS = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
        
        if (rank == 0)
        {
            printf("MPI Merge sort time spent = %12.10f\n",mpiMS);

            // Array checking should only be done in root process
            if (arrayCheck(libSort, mpiMergeSort, arrLen))
            {
                printf("MPI Merge sort algo broken\n\n");
                exit(EXIT_FAILURE);
            }
            else{
                printf("MPI Merge sort runs successfully\n\n");
            }

            printf("Merge sorting MPI time reduction (percent): %lf \n\n", percentReduct(serialMS, mpiMS));
        }

        free(mpiMergeSort);
    }

    // QUICK SORT ACTION
    if(action[0] == 'a' || action[0] == 'q' || action[0] == 'd'){
        // Serial quick sort check
        double serialQS = (double)0;
        if (rank == 0)
        {
            gettimeofday(&start, NULL);
            double *serialQuickS = quickSort(ogArr, arrLen);
            gettimeofday(&end, NULL);

            serialQS = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
            printf("Serial Quick sort time spent = %12.10f\n",serialQS);

            if (arrayCheck(libSort, serialQuickS, arrLen))
            {
                printf("Serial Quick sort algo broken\n\n");
                exit(EXIT_FAILURE);
            }
            else{
                printf("Serial Quick sort runs successfully\n\n");
            }

            free(serialQuickS);
        }
        
        MPI_Barrier(MPI_COMM_WORLD);

        // MPI Quick sort check...
        gettimeofday(&start, NULL);
        double *mpiQuickS = mpiSort(ogArr, arrLen, procArSize, "quick");
        gettimeofday(&end, NULL);

        double mpiQS = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;

        if (rank == 0)
        {
            printf("MPI Quick sort time spent = %12.10f\n",mpiQS);

            // Array checking should only be done in root process
            if (arrayCheck(libSort, mpiQuickS, arrLen))
            {
                printf("MPI Quick sort algo broken\n\n");
                exit(EXIT_FAILURE);
            }
            else{
                printf("MPI Quick sort runs successfully\n\n");
            }

            printf("Quick sorting MPI time reduction (percent): %lf \n\n", percentReduct(serialQS, mpiQS));
        }
        
        free(mpiQuickS);
    }

    // ENUMERATION SORT ACTION
    if(action[0] == 'a' || action[0] == 'e'){
        // Serial enum sort check
        double serialES = (double)0;
        if (rank == 0)
        {
            gettimeofday(&start, NULL);
            double *serialEnumS = enumSort(ogArr, arrLen);
            gettimeofday(&end, NULL);

            serialES = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
            printf("Serial Enum sort time spent = %12.10f\n",serialES);

            if (arrayCheck(libSort, serialEnumS, arrLen))
            {
                printf("Serial Enum sort algo broken\n\n");
                exit(EXIT_FAILURE);
            }
            else{
                printf("Serial Enum sort runs successfully\n\n");
            }
        }
        
        MPI_Barrier(MPI_COMM_WORLD);

        // Enumeration sort check
        gettimeofday(&start, NULL);
        double *mpiEnumS = mpiSort(ogArr, arrLen, procArSize, "enum");
        gettimeofday(&end, NULL);

        double mpiES = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;

        if (rank == 0)
        {
            printf("MPI Enum sort time spent = %12.10f\n",mpiES);

            // Array checking should only be done in root process
            if (arrayCheck(libSort, mpiEnumS, arrLen))
            {
                printf("MPI Enum sort algo broken\n\n");
                exit(EXIT_FAILURE);
            }
            else{
                printf("MPI Enum sort runs successfully\n\n");
            }

            printf("Enumeration sorting MPI time reduction (percent): %lf \n\n", percentReduct(serialES, mpiES));
        }
        
        free(mpiEnumS);
    }

    return 0;
}

/*
Function to run the project after main had check and approved the inputs from user
Purpose to make main cleaner and handling only inputs
This function runs once every IO operations and sortings : serial and mpi
Parameters: number of omp threads, array size, action to perform
*/
int projectHandlr(int numThreads, int arrLen, char *action)
{
    omp_set_num_threads(numThreads);

    srand(time(NULL));

    int rank, parallelNum;
    // Get rank of process
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &parallelNum);

    if (rank == 0)
    {
        printf("Project 2 normal run with MPI size: %d; omp num threads: %d ; array size: %d ; action: %s\n\n", parallelNum, omp_get_max_threads(), arrLen, action);
    }
    
    MPI_Barrier(MPI_COMM_WORLD);

    // Determine each process sub array size
    int *procArSize = processArrSize(arrLen);

    // Determine each process sub array start index
    int *displacements = subArStartAt(arrLen);

    double *mpiOgArr = ioRun(arrLen, procArSize, displacements);

    free(displacements);
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("-------------------------------------------------\n");
        printf("Project Sorting algo starts\n");
        printf("-------------------------------------------------\n\n");
    }

    MPI_Barrier(MPI_COMM_WORLD);

    sortRun(mpiOgArr, arrLen, action, procArSize);

    MPI_Barrier(MPI_COMM_WORLD);

    return 0;
}

void giveHelp()
{
    printf("Wrong input format and/or type\n");
    printf("Please type \"make help\" for more imformation\n");
}

// Main purpose is to clean inputs only
// For instruction on how to compile and run. Refers to makefile
// Or type "make help" in the console
int main(int argc, char *argv[]) 
{
    int rank;

    MPI_Init(&argc, &argv);
    
    // Get rank of process and size of comm
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Barrier(MPI_COMM_WORLD);
    
    // CLEAN UP INPUTS
    if (argc < 5){
        giveHelp();

        exit(EXIT_FAILURE);
    }
    else
    {
        // Correct amount of input
        // Check for input TYPE
        if (isInteger(argv[2]) == 0 || isInteger(argv[3]) == 0 || (argv[4][0] != 'd' && argv[4][0] != 'q' && argv[4][0] != 'm' && argv[4][0] != 'a' && argv[4][0] != 'e'))
        {
            giveHelp();
            exit(EXIT_FAILURE);
        }

        // Inputs are clean
        // Tranform into correct type
        int numThreads = atoi(argv[2]);
        int arrLen = atoi(argv[3]);
        char *action = argv[4];
        int timesRun = 10;

        // RUN THE PROJECT
        MPI_Barrier(MPI_COMM_WORLD);

        if (strcmp("run", argv[1]) == 0)
        {
            // Normal run
            // Run both IO and Sorting
            projectHandlr(numThreads, arrLen, action);

            MPI_Barrier(MPI_COMM_WORLD);
        }
        else if (strcmp("reportIO", argv[1]) == 0)
        {
            // Report run for IO operation only
            reportIO(numThreads, arrLen, action, timesRun);

            MPI_Barrier(MPI_COMM_WORLD);
        }
        else if (strcmp("report", argv[1]) == 0)
        {
            // Report run for sorting algorithms
            sortReport(numThreads, arrLen, action, timesRun);
        }
        else
        {
            // Unrecognized run command
            exit(EXIT_FAILURE);
        }
        
        MPI_Finalize();

        exit(EXIT_SUCCESS);
        return 0;
    }
}
