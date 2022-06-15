#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

void bubbleSort(int *array, int n) {
    int temp;
    for (int k = 1; k < n; k++) {
        for (int j = 0; j < n - k; j++) {
            if (array[j] > array[j + 1]) {
                temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
}

void populateArray(int *array, int n) {
    for (int i = 0; i < n; i++) {
        array[i] = rand() % 100;
    }
}

void printArray(int *array, int n) {
    printf("[ ");
    for (int i = 0; i < n; i++) {
        printf("%d ", array[i]);
    }
    printf("]\n");
}

void compareExchangeRight(int *innerArray, int *innerArrayToSort, int elementsPerArray, int rank) {
    MPI_Send(innerArray, elementsPerArray, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
    MPI_Recv(innerArrayToSort + elementsPerArray, elementsPerArray, MPI_INT, rank + 1, 0, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
    bubbleSort(innerArrayToSort, elementsPerArray * 2);
    memcpy(innerArray, innerArrayToSort, elementsPerArray * sizeof(int));
}

void compareExchangeLeft(int *innerArray, int *innerArrayToSort, int elementsPerArray, int rank) {
    MPI_Recv(innerArrayToSort + elementsPerArray, elementsPerArray, MPI_INT, rank - 1, 0, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
    MPI_Send(innerArray, elementsPerArray, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
    bubbleSort(innerArrayToSort, elementsPerArray * 2);
    memcpy(innerArray, innerArrayToSort + elementsPerArray, elementsPerArray * sizeof(int));
}

int main(int argc, char **argv) {
    int rank, size;
    int arraySize = atoi(argv[1]);
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int initialArray[arraySize];
    int elementsPerArray = arraySize / size;
    int innerArray[elementsPerArray];
    int innerArrayToSort[elementsPerArray * 2];
    if (rank == 0) {
        populateArray(initialArray, arraySize);
        printf("Array to be sorted: ");
        printArray(initialArray, arraySize);
    }
    MPI_Scatter(initialArray, elementsPerArray, MPI_INT, innerArray, elementsPerArray, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    for (int i = 0; i < size; i++) {
        MPI_Barrier(MPI_COMM_WORLD);
        memcpy(innerArrayToSort, innerArray, elementsPerArray * sizeof(int));
        if (i % 2 == 0) { // Even iteration
            if (rank % 2 == 0) { // Even process number
                if (rank < size - 1) {
                    // Compare-exchange with the right neighbor process
                    compareExchangeRight(innerArray, innerArrayToSort, elementsPerArray, rank);
                }
            } else { // Odd process number
                if (rank > 0) {
                    // Compare-exchange with the left neighbor process
                    compareExchangeLeft(innerArray, innerArrayToSort, elementsPerArray, rank);
                }
            }
        } else { // Odd iteration
            if (rank % 2 == 1) { // Odd process number
                if (rank < size - 1) {
                    // Compare-exchange with the right neighbor process
                    compareExchangeRight(innerArray, innerArrayToSort, elementsPerArray, rank);
                }
            } else { // Even process number
                if (rank > 0) {
                    // Compare-exchange with the left neighbor process
                    compareExchangeLeft(innerArray, innerArrayToSort, elementsPerArray, rank);
                }
            }
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gather(innerArray, elementsPerArray, MPI_INT, initialArray, elementsPerArray, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        printf("Sorted array: ");
        printArray(initialArray, arraySize);
    }
    MPI_Finalize();
    return 0;
}