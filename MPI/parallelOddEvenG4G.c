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

void populateArray(int *array, int n) {
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        array[i] = rand() % 100;
    }
}

int isSanityChecked(int *array, int *sane, int n) {
    for (int i = 0; i < n; i++) {
        if (array[i] != sane[i])
            return 0;
    }
    return 1;
}

void printArray(int *array, int n) {
    printf("[ ");
    for (int i = 0; i < n; i++) {
        printf("%d ", array[i]);
    }
    printf("]\n");
}

void compareExchange(int *array, int posWithoutOdd, int odd) {
    int pos = posWithoutOdd + odd;
    if (array[pos] > array[pos + 1]) {
        int temp = array[pos];
        array[pos] = array[pos + 1];
        array[pos + 1] = temp;
    }
}

int main(int argc, char **argv) {
    int rank, size;
    int arraySize = atoi(argv[1]);
    int initialArray[arraySize];
    int saneArray[arraySize];
    double duration = 0.0;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        printf("Parallel OddEven GeeksForGeeks Version\n");
        populateArray(initialArray, arraySize);
        memcpy(saneArray, initialArray, arraySize * sizeof(int));
        // printf("Array to be sorted: ");
        // printArray(initialArray, arraySize);
        mergeSort(saneArray, 0, arraySize - 1);
        duration -= MPI_Wtime();
    }
    MPI_Bcast(initialArray, arraySize, MPI_INT, 0, MPI_COMM_WORLD);

    int displacements[size], counts[size];

    for (int i = 0; i < size; i++) {
        displacements[i] = (arraySize / size) * i + 1;
        counts[i] = arraySize / size - (i == size - 1);
    }

    int nIterationsPerProcess = arraySize / (size * 2);

    // One even and one odd iteration for each i in arraySize
    for (int i = 0; i < arraySize / 2; i++) {
        int pos;

        int eachProccessChunkSize = 2 * nIterationsPerProcess;

        int startingPos = rank * eachProccessChunkSize;

        for (int j = 0; j < eachProccessChunkSize; j += 2) {
            pos = startingPos + j;
            compareExchange(initialArray, pos, 0);
        }

        MPI_Allgather(initialArray + startingPos, eachProccessChunkSize, MPI_INT, initialArray, eachProccessChunkSize,
                      MPI_INT, MPI_COMM_WORLD);

        for (int j = 0; j < eachProccessChunkSize; j += 2) {
            pos = startingPos + j;
            if (pos + 1 != arraySize - 1)
                compareExchange(initialArray, pos, 1);
        }

        MPI_Allgatherv(initialArray + startingPos + 1, counts[rank], MPI_INT, initialArray, counts, displacements,
                       MPI_INT, MPI_COMM_WORLD);
    }

    if (rank == 0) {
        duration += MPI_Wtime();
        // printf("Sorted array: ");
        // printArray(initialArray, arraySize);
        printf("Duration: %lfs\n", duration);
        printf("Is it correct? %d\n", isSanityChecked(initialArray, saneArray, arraySize));
    }

    MPI_Finalize();
    return 0;
}