#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

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

void printArray(int *array, int n) {
    printf("[ ");
    for (int i = 0; i < n; i++) {
        printf("%d ", array[i]);
    }
    printf("]\n");
}

int main(int argc, char **argv) {
    int rank, size;
    int *inner_array = malloc(sizeof(int) * 16);
    int initial_array[16] = {13, 55, 59, 88, 29, 43, 71, 85, 2, 18, 40, 75, 4, 14, 22, 43};
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (rank == 0) {
        // read / generate the initial array here
        printf("Array to be sorted: ");
        printArray(initial_array, 16);
    }
    MPI_Scatter(initial_array, 4, MPI_INT, inner_array, 4, MPI_INT, 0, MPI_COMM_WORLD);
//    printf("Array block in process %d ", rank);
//    printArray(inner_array, 4);
    MPI_Barrier(MPI_COMM_WORLD);
    for (int i = 0; i < size; i++) {
        int inner_array_to_sort[8];
//        printf("Process %d iteration %d\n", rank, i);
        MPI_Barrier(MPI_COMM_WORLD);
        memcpy(inner_array_to_sort, inner_array, 4 * sizeof(int));
        if (i % 2 == 0) { // Even iteration
            if (rank % 2 == 0) { // Even process number
                if (rank < size - 1) {
                    // Compare-exchange with the right neighbor process
                    MPI_Send(inner_array, 4, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
                    MPI_Recv(inner_array_to_sort + 4, 4, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    mergeSort(inner_array_to_sort, 0, 7);
                    memcpy(inner_array, inner_array_to_sort, 4 * sizeof(int));
//                    printArray(inner_array, 4);
//                    printf("%d %d Compare-exchange with the right neighbor process\n", i, rank);
                }
            } else { // Odd process number
                if (rank > 0) {
                    // Compare-exchange with the left neighbor process
                    MPI_Recv(inner_array_to_sort + 4, 4, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Send(inner_array, 4, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
                    mergeSort(inner_array_to_sort, 0, 7);
                    memcpy(inner_array, inner_array_to_sort + 4, 4 * sizeof(int));
//                    printArray(inner_array, 4);
//                    printf("%d %d Compare-exchange with the left neighbor process\n", i, rank);
                }
            }
        } else { // Odd iteration
            if (rank % 2 == 1) { // Odd process number
                if (rank < size - 1) {
                    // Compare-exchange with the right neighbor process
                    MPI_Send(inner_array, 4, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
                    MPI_Recv(inner_array_to_sort + 4, 4, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    mergeSort(inner_array_to_sort, 0, 7);
                    memcpy(inner_array, inner_array_to_sort, 4 * sizeof(int));
//                    printArray(inner_array, 4);
//                    printf("%d %d Compare-exchange with the right neighbor process\n", i, rank);
                }
            } else { // Even process number
                if (rank > 0) {
                    // Compare-exchange with the left neighbor process
                    MPI_Recv(inner_array_to_sort + 4, 4, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Send(inner_array, 4, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
                    mergeSort(inner_array_to_sort, 0, 7);
                    memcpy(inner_array, inner_array_to_sort + 4, 4 * sizeof(int));
//                    printArray(inner_array, 4);
//                    printf("%d %d Compare-exchange with the left neighbor process\n", i, rank);
                }
            }
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    printf("Inner Array process %d: ", rank);
    MPI_Gather(inner_array, 4, MPI_INT, initial_array, 4, MPI_INT, 0, MPI_COMM_WORLD);
    printArray(inner_array, 4);
    if (rank == 0) {
        printf("Sorted array: ");
        printArray(initial_array, 16);
    }
    free(inner_array);
    MPI_Finalize();
    return 0;
}