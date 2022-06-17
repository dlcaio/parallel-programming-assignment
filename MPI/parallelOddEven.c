#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <time.h>

void merge(int arr[], int p, int q, int r) {
    // Create L ← A[p..q] and M ← A[q+1..r]
    int n1 = q - p + 1;
    int n2 = r - q;

    int L[n1], M[n2];

    for (int i = 0; i < n1; i++)
        L[i] = arr[p + i];
    for (int j = 0; j < n2; j++)
        M[j] = arr[q + 1 + j];

    // Maintain current index of sub-arrays and main array
    int i, j, k;
    i = 0;
    j = 0;
    k = p;

    // Until we reach either end of either L or M, pick larger among
    // elements L and M and place them in the correct position at A[p..r]
    while (i < n1 && j < n2) {
        if (L[i] <= M[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = M[j];
            j++;
        }
        k++;
    }

    // When we run out of elements in either L or M,
    // pick up the remaining elements and put in A[p..r]
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = M[j];
        j++;
        k++;
    }
}

// Source: https://www.programiz.com/dsa/merge-sort
// Divide the array into two subarrays, sort them and merge them
void mergeSort(int arr[], int l, int r) {
    if (l < r) {

        // m is the point where the array is divided into two subarrays
        int m = l + (r - l) / 2;

        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);

        // Merge the sorted subarrays
        merge(arr, l, m, r);
    }
}

int isSanityChecked(int *array, int *sane, int n) {
    for (int i = 0; i < n; i++) {
        if (array[i] != sane[i])
            return 0;
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

void compareExchangeRight(int *innerArray, int *incomingArray, int elementsPerArray, int rank) {
    MPI_Send(innerArray, elementsPerArray, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
    MPI_Recv(incomingArray, elementsPerArray, MPI_INT, rank + 1, 0, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
}

void compareExchangeLeft(int *innerArray, int *incomingArray, int elementsPerArray, int rank) {
    MPI_Recv(incomingArray, elementsPerArray, MPI_INT, rank - 1, 0, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
    MPI_Send(innerArray, elementsPerArray, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
}

void mergeArrays(int *array1, int *array2, int *resultArray, int elementsPerArray) {
    int i = 0, j = 0, k = 0;
    while (i < elementsPerArray && j < elementsPerArray) {
        if (array1[i] <= array2[j]) {
            resultArray[k] = array1[i];
            i++;
        } else {
            resultArray[k] = array2[j];
            j++;
        }
        k++;
    }

    while (i < elementsPerArray) {
        resultArray[k] = array1[i];
        i++;
        k++;
    }

    while (j < elementsPerArray) {
        resultArray[k] = array2[j];
        j++;
        k++;
    }
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
    int incomingArray[elementsPerArray];
    int saneArray[arraySize];
    double duration = 0.0;
    if (rank == 0) {
        printf("Parallel OddEven Chunks Version\n");
        populateArray(initialArray, arraySize);
        memcpy(saneArray, initialArray, arraySize * sizeof(int));
//        printf("Array to be sorted: ");
//        printArray(initialArray, arraySize);
        mergeSort(saneArray, 0, arraySize - 1);
        duration -= MPI_Wtime();
    }
    MPI_Scatter(initialArray, elementsPerArray, MPI_INT, innerArray, elementsPerArray, MPI_INT, 0, MPI_COMM_WORLD);
    mergeSort(innerArray, 0, elementsPerArray - 1);
    for (int i = 0; i < size; i++) {
        if (i % 2 == 0) { // Even iteration
            if (rank % 2 == 0) { // Even process number
                if (rank < size - 1) {
                    // Compare-exchange with the right neighbor process
                    compareExchangeRight(innerArray, incomingArray, elementsPerArray, rank);
                    mergeArrays(innerArray, incomingArray, innerArrayToSort, elementsPerArray);
                    memcpy(innerArray, innerArrayToSort, elementsPerArray * sizeof(int));
                }
            } else { // Odd process number
                if (rank > 0) {
                    // Compare-exchange with the left neighbor process
                    compareExchangeLeft(innerArray, incomingArray, elementsPerArray, rank);
                    mergeArrays(innerArray, incomingArray, innerArrayToSort, elementsPerArray);
                    memcpy(innerArray, innerArrayToSort + elementsPerArray, elementsPerArray * sizeof(int));
                }
            }
        } else { // Odd iteration
            if (rank % 2 == 1) { // Odd process number
                if (rank < size - 1) {
                    // Compare-exchange with the right neighbor process
                    compareExchangeRight(innerArray, incomingArray, elementsPerArray, rank);
                    mergeArrays(innerArray, incomingArray, innerArrayToSort, elementsPerArray);
                    memcpy(innerArray, innerArrayToSort, elementsPerArray * sizeof(int));
                }
            } else { // Even process number
                if (rank > 0) {
                    // Compare-exchange with the left neighbor process
                    compareExchangeLeft(innerArray, incomingArray, elementsPerArray, rank);
                    mergeArrays(innerArray, incomingArray, innerArrayToSort, elementsPerArray);
                    memcpy(innerArray, innerArrayToSort + elementsPerArray, elementsPerArray * sizeof(int));
                }
            }
        }
    }
    MPI_Gather(innerArray, elementsPerArray, MPI_INT, initialArray, elementsPerArray, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        duration += MPI_Wtime();
//        printf("Sorted array: ");
//        printArray(initialArray, arraySize);
//        printf("Sane array: ");
//        printArray(saneArray, arraySize);
        printf("Duration: %lfs\n", duration);
        printf("Is it correct? %d\n", isSanityChecked(initialArray, saneArray, arraySize));
    }
    MPI_Finalize();
    return 0;
}