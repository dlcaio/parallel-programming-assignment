#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <time.h>

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

int isSorted(int *array, int n) {
    int prevElem = array[0];
    for (int i = 1; i < n; i++) {
        if (prevElem > array[i]) {
            return 0;
        }
        prevElem = array[i];
    }
    return 1;
}

void populateArray(int *array, int n) {
    srand(time(NULL));
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
    int arraySize = atoi(argv[1]);
    MPI_Init(&argc, &argv);
    int initialArray[arraySize];
    double duration = 0.0;
    printf("Sequential Bubble Sort Version\n");
    populateArray(initialArray, arraySize);
    //        printf("Array to be sorted: ");
    //        printArray(initialArray, arraySize);
    duration -= MPI_Wtime();
    bubbleSort(initialArray, arraySize);
    duration += MPI_Wtime();
    //        printf("Sorted array: ");
    //        printArray(initialArray, arraySize);
    printf("Duration: %lfs\n", duration);
    printf("Is it sorted? %d\n", isSorted(initialArray, arraySize));
    MPI_Finalize();
    return 0;
}
