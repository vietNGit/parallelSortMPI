#include "ioWork.h"

#include "mpi.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include "extraFunc.h"

#define unsortFile "unsorted_array.bin"
#define mpiUSortFile "mpi_unsorted.bin"

void notExist(char *fName)
{
    printf("File %s does not exist!\n", fName);
}

int writeToBin(double *arr, int arrLen, char *fName)
{
    FILE *fp = fopen(fName, "wb");
    
    // Write entire array to binary file
    fwrite(arr, sizeof(double), arrLen, fp);
    
    fclose(fp);

    return 0;
}

int compareFromFile(double *arr, int arrLen, char *fName)
{
    if( access( fName, F_OK ) == 0 ) 
    {
        // file exists
        FILE *fp = fopen(fName, "rb");
    
        // Compare array
        double *comAr = malloc(arrLen* sizeof(double));

        fread(comAr, sizeof(double), arrLen, fp);
        
        if (arrayCheck(arr, comAr, arrLen))
        {
            printf("Binary files array not match with original array\n");
            fclose(fp);

            free(comAr);

            return 1;
        }
        else{
            fclose(fp);

            free(comAr);

            return 0;
        }
    } 
    else 
    {
        // file doesn't exist
        notExist(fName);
        return 1;
    }
}

int readFromBin(double *reArr, int arrLen, char *fName)
{
    // reArr array should already be of arrLen
    if( access( fName, F_OK ) == 0 ) 
    {
        // file exists
        FILE *fp = fopen(fName, "rb");

        fread(reArr, sizeof(double), arrLen, fp);
        
        fclose(fp);

        return 0;
    } 
    else 
    {
        // file doesn't exist
        notExist(fName);
        return 1;
    }
    return 0;
}

int mpiWriteFile(int arrLen, double *ogArr, char *fName, int *procArSize, int *displacements)
{
    // Check if file can be open
    // file exists
    MPI_File fh; //Declaring a File Pointer
    MPI_Status status;
    MPI_File_open(MPI_COMM_WORLD, fName, MPI_MODE_CREATE|MPI_MODE_WRONLY, MPI_INFO_NULL, &fh);
    
    int rank, parallelNum;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &parallelNum);

    MPI_File_write_at(fh, displacements[rank] * sizeof(double), ogArr + displacements[rank], procArSize[rank], MPI_DOUBLE, &status);

    MPI_File_close(&fh);

    return 0;
}

int mpiReadFile(int arrLen, double *reArr, char *fName, int *procArSize, int *displacements)
{
    // reArr array should already be of arrLen
    // Check if file can be open
    if( access( fName, F_OK ) == 0 ) 
    {
        // file exists
        MPI_File fh; //Declaring a File Pointer
        MPI_Status status;
        MPI_File_open(MPI_COMM_WORLD, fName, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);

        int rank, parallelNum;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &parallelNum);
        
        MPI_File_read_at(fh, displacements[rank] * sizeof(double), reArr + displacements[rank], procArSize[rank], MPI_DOUBLE, &status);

        // Send and receive the full array for every process
        if (rank == 0)
        {
            MPI_Gatherv(reArr + displacements[rank], procArSize[rank], MPI_DOUBLE, reArr, procArSize, displacements, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        }
        else
        {
            MPI_Gatherv(reArr + displacements[rank], procArSize[rank], MPI_DOUBLE, NULL, NULL, NULL, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        }

        MPI_File_close(&fh);

        return 0;
    } 
    else 
    {
        // file doesn't exist
        notExist(fName);
        return 1;
    }
}

double *ioRun(int arrLen, int *procArSize, int *displacements){
    int rank, parallelNum;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &parallelNum);

    MPI_Barrier(MPI_COMM_WORLD);

    // ARRAY GENERATION SHOULD BE DONE ONLY BY ROOT PROCESS
    // Every process allocate memory for generated array
    double *ogArr = malloc(arrLen * sizeof(double));
    if (rank == 0)
    {
        // Array generated
        arrayGen(ogArr, arrLen);
    }

    // Root sends generated array to everyone
    MPI_Bcast(ogArr, arrLen, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    struct timeval start, end;

    // WRITE OPERATION
    // Serial write operation
    double serialWrite = (double)0;
    if (rank == 0)
    {
        // Serial write operation
        gettimeofday(&start, NULL);
        writeToBin(ogArr, arrLen, unsortFile);
        gettimeofday(&end, NULL);

        serialWrite = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
        printf("Serial file writing time spent = %12.10f\n\n",serialWrite);

        printf("Write operation check:\n");
        if (compareFromFile(ogArr, arrLen, unsortFile) == 1)
        {
            // File compare failure
            printf("Write operation failure\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            printf("Write operation success\n\n");
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    // MPI write to file
    gettimeofday(&start, NULL);
    mpiWriteFile(arrLen, ogArr, mpiUSortFile, procArSize, displacements);
    gettimeofday(&end, NULL);

    if (rank == 0)
    {
        double mpiWrite = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
        printf("MPI file writing time spent = %12.10f\n\n",mpiWrite);

        printf("MPI Write operation check:\n");
        if (compareFromFile(ogArr, arrLen, mpiUSortFile) == 1)
        {
            // File compare failure
            printf("MPI Write operation failure\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            printf("MPI Write operation success\n\n");
        }

        printf("Write operation MPI time reduction (percent): %lf \n\n", percentReduct(serialWrite, mpiWrite));
    }  
    MPI_Barrier(MPI_COMM_WORLD);

    // Original array receive from array gen in serial no longer needed
    // Because we confirm the array written to file is correct
    // Therefore freeing it to prevent memory overload
    free(ogArr);

    MPI_Barrier(MPI_COMM_WORLD);

    // READ OPERATION
    // Serial read operation
    double serialRead = (double)0;
    if (rank == 0)
    {
        // Get array from the binary file
        double *arr = malloc(arrLen * sizeof(double));
        gettimeofday(&start, NULL);
        readFromBin(arr, arrLen, unsortFile);
        gettimeofday(&end, NULL);

        serialRead = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
        printf("Serial file reading time spent = %12.10f\n\n",serialRead);

        printf("Read operation check:\n");
        if (compareFromFile(arr, arrLen, unsortFile) == 1)
        {
            // File compare failure
            printf("Read operation failure\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            printf("Read operation success\n\n");
        }
    }

    // MPI read from file
    double *mpiOgArr = malloc(arrLen * sizeof(double));

    gettimeofday(&start, NULL);
    mpiReadFile(arrLen, mpiOgArr, mpiUSortFile, procArSize, displacements);
    gettimeofday(&end, NULL);

    if (rank == 0)
    {
        double mpiRead = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
        printf("MPI file reading time spent = %12.10f\n\n",mpiRead);

        printf("MPI Read operation check:\n");
        if (compareFromFile(mpiOgArr, arrLen, mpiUSortFile) == 1)
        {
            // File compare failure
            printf("MPI Read operation failure\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            printf("MPI Read operation success\n\n");
        }

        printf("Read operation MPI time reduction (percent): %lf \n\n", percentReduct(serialRead, mpiRead));
    }
    MPI_Bcast(mpiOgArr, arrLen, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    
    // IO operation finished
    // Returns generated array, every process should receive
    return mpiOgArr;
}

double *ioReportRun(int arrLen, int *procArSize, int *displacements)
{
    // Prepare time array
    double *timeArray = malloc(4 * sizeof(double));

    int rank, parallelNum;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &parallelNum);

    MPI_Barrier(MPI_COMM_WORLD);

    // ARRAY GENERATION SHOULD BE DONE ONLY BY ROOT PROCESS
    // Every process allocate memory for generated array
    double *ogArr = malloc(arrLen * sizeof(double));
    if (rank == 0)
    {
        // Array generated
        arrayGen(ogArr, arrLen);
    }

    // Root sends generated array to everyone
    MPI_Bcast(ogArr, arrLen, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    struct timeval start, end;

    // WRITE OPERATION
    // Serial write operation
    if (rank == 0)
    {
        // Serial write operation
        gettimeofday(&start, NULL);
        writeToBin(ogArr, arrLen, unsortFile);
        gettimeofday(&end, NULL);

        timeArray[0] = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;

        if (compareFromFile(ogArr, arrLen, unsortFile) == 1)
        {
            // File compare failure
            printf("Write operation failure\n");
            exit(EXIT_FAILURE);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    // MPI write to file
    gettimeofday(&start, NULL);
    mpiWriteFile(arrLen, ogArr, mpiUSortFile, procArSize, displacements);
    gettimeofday(&end, NULL);

    if (rank == 0)
    {
        timeArray[1] = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;

        if (compareFromFile(ogArr, arrLen, mpiUSortFile) == 1)
        {
            // File compare failure
            printf("MPI Write operation failure\n");
            exit(EXIT_FAILURE);
        }
    }  
    MPI_Barrier(MPI_COMM_WORLD);

    // Original array receive from array gen in serial no longer needed
    // Because we confirm the array written to file is correct
    // Therefore freeing it to prevent memory overload
    free(ogArr);

    MPI_Barrier(MPI_COMM_WORLD);

    // READ OPERATION
    // Serial read operation
    if (rank == 0)
    {
        // Get array from the binary file
        double *arr = malloc(arrLen * sizeof(double));
        gettimeofday(&start, NULL);
        readFromBin(arr, arrLen, unsortFile);
        gettimeofday(&end, NULL);

        timeArray[2] = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;

        if (compareFromFile(arr, arrLen, unsortFile) == 1)
        {
            // File compare failure
            printf("Read operation failure\n");
            exit(EXIT_FAILURE);
        }

        free(arr);
    }

    // MPI read from file
    double *mpiOgArr = malloc(arrLen * sizeof(double));

    gettimeofday(&start, NULL);
    mpiReadFile(arrLen, mpiOgArr, mpiUSortFile, procArSize, displacements);
    gettimeofday(&end, NULL);

    if (rank == 0)
    {
        timeArray[3] = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;

        if (compareFromFile(mpiOgArr, arrLen, mpiUSortFile) == 1)
        {
            // File compare failure
            printf("MPI Read operation failure\n");
            exit(EXIT_FAILURE);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    
    free(mpiOgArr);

    // IO operation finished
    // Returns time array
    return timeArray;
}