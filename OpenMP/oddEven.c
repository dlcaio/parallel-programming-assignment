#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>

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

void compareExchange(int *array, int pos) {
    if (array[pos] > array[pos + 1]) {
        int temp = array[pos];
        array[pos] = array[pos + 1];
        array[pos + 1] = temp;
    }
}

int isSanityChecked(int *array, int *sane, int n) {
    for (int i = 0; i < n; i++) {
        if (array[i] != sane[i])
            return 0;
    }
    return 1;
}

int main(int argc, char **argv) {
    int arraySize = atoi(argv[1]);
    int initialArray[arraySize];
    int numThreads = atoi(argv[2]);
    int saneArray[arraySize];
    omp_set_num_threads(numThreads);
    printf("OpenMP Bubble Sort Version\n");
    populateArray(initialArray, arraySize);
    memcpy(saneArray, initialArray, arraySize * sizeof(int));
    mergeSort(saneArray, 0, arraySize - 1);
//    printf("Array to be sorted: ");
//    printArray(initialArray, arraySize);
    double start = omp_get_wtime();
    for (int i = 0; i < arraySize; i++) {
        if (i % 2 == 1) {
            #pragma omp parallel for
            for (int j = 0; j < arraySize / 2 - 1; j++) {
                compareExchange(initialArray, 2 * j + 1);
            }
        } else {
            #pragma omp parallel for
            for (int j = 0; j < arraySize / 2; j++) {
                compareExchange(initialArray, 2 * j);
            }
        }
    }
    double end = omp_get_wtime();
    double duration = end - start;
//    printf("Sorted array: ");
//    printArray(initialArray, arraySize);
    printf("Duration: %lfs\n", duration);
    printf("Is it correct? %d\n", isSanityChecked(initialArray, saneArray, arraySize));
    return 0;
}
