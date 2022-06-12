#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

void bubbleSort(int *array, int n) {
    int temp;
    for (int k = 1; k < n; k++) {
        for (int j = 0; j < n - 1; j++) {
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

int main(int argc, char **argv) {
    int rank, size;
    int arraySize = atoi(argv[1]);
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int initialArray[arraySize];
    int elementsPerArray = arraySize / size;
    int innerArray[elementsPerArray];
    if (rank == 0) {
        populateArray(initialArray, arraySize);
        printf("Array to be sorted: ");
        printArray(initialArray, arraySize);
    }
    MPI_Scatter(initialArray, elementsPerArray, MPI_INT, innerArray, elementsPerArray, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    for (int i = 0; i < size; i++) {
        int innerArray_to_sort[elementsPerArray * 2];
        MPI_Barrier(MPI_COMM_WORLD);
        memcpy(innerArray_to_sort, innerArray, elementsPerArray * sizeof(int));
        if (i % 2 == 0) { // Even iteration
            if (rank % 2 == 0) { // Even process number
                if (rank < size - 1) {
                    // Compare-exchange with the right neighbor process
                    MPI_Send(innerArray, elementsPerArray, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
                    MPI_Recv(innerArray_to_sort + elementsPerArray, elementsPerArray, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    bubbleSort(innerArray_to_sort, elementsPerArray * 2);
                    memcpy(innerArray, innerArray_to_sort, elementsPerArray * sizeof(int));
                }
            } else { // Odd process number
                if (rank > 0) {
                    // Compare-exchange with the left neighbor process
                    MPI_Recv(innerArray_to_sort + elementsPerArray, elementsPerArray, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Send(innerArray, elementsPerArray, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
                    bubbleSort(innerArray_to_sort, elementsPerArray * 2);
                    memcpy(innerArray, innerArray_to_sort + elementsPerArray, elementsPerArray * sizeof(int));
                }
            }
        } else { // Odd iteration
            if (rank % 2 == 1) { // Odd process number
                if (rank < size - 1) {
                    // Compare-exchange with the right neighbor process
                    MPI_Send(innerArray, elementsPerArray, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
                    MPI_Recv(innerArray_to_sort + elementsPerArray, elementsPerArray, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    bubbleSort(innerArray_to_sort, elementsPerArray * 2);
                    memcpy(innerArray, innerArray_to_sort, elementsPerArray * sizeof(int));
                }
            } else { // Even process number
                if (rank > 0) {
                    // Compare-exchange with the left neighbor process
                    MPI_Recv(innerArray_to_sort + elementsPerArray, elementsPerArray, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Send(innerArray, elementsPerArray, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
                    bubbleSort(innerArray_to_sort, elementsPerArray * 2);
                    memcpy(innerArray, innerArray_to_sort + elementsPerArray, elementsPerArray * sizeof(int));
                }
            }
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gather(innerArray, elementsPerArray, MPI_INT, initialArray, elementsPerArray, MPI_INT, 0, MPI_COMM_WORLD);
    printf("Inner Array process %d: ", rank);
    printArray(innerArray, elementsPerArray);
    if (rank == 0) {
        printf("Sorted array: ");
        printArray(initialArray, arraySize);
    }
    MPI_Finalize();
    return 0;
}