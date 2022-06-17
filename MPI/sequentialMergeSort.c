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
    printf("Sequential Merge Sort Version\n");
    populateArray(initialArray, arraySize);
    //        printf("Array to be sorted: ");
    //        printArray(initialArray, arraySize);
    duration -= MPI_Wtime();
    mergeSort(initialArray, 0, arraySize - 1);
    duration += MPI_Wtime();
    //        printf("Sorted array: ");
    //        printArray(initialArray, arraySize);
    printf("Duration: %lfs\n", duration);
    printf("Is it sorted? %d\n", isSorted(initialArray, arraySize));
    MPI_Finalize();
    return 0;
}
