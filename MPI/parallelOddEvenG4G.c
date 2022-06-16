#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <time.h>

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
    double duration = 0.0;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        printf("Parallel OddEven GeeksForGeeks Version\n");
        populateArray(initialArray, arraySize);
//        printf("Array to be sorted: ");
//        printArray(initialArray, arraySize);
        duration -= MPI_Wtime();
    }
    MPI_Bcast(initialArray, arraySize, MPI_INT, 0, MPI_COMM_WORLD);

    int nIterationsPerProcess = arraySize / (size * 2);

    // One even and one odd iteration for each i in arraySize
    for (int i = 0; i < arraySize / 2; i++) {
        for (int j = 0; j < nIterationsPerProcess; j++) {
            int pos = (rank * 2) + j * 2 * size;

            compareExchange(initialArray, pos, 0);
            MPI_Allgather(initialArray + pos, 2, MPI_INT, initialArray + j * 2 * size, 2, MPI_INT, MPI_COMM_WORLD);

            
        }

        for (int j = 0; j < nIterationsPerProcess; j++) {
          int pos = (rank * 2) + j * 2 * size;

          MPI_Comm ODD_COMM;
            if (j == nIterationsPerProcess - 1)
                MPI_Comm_split(MPI_COMM_WORLD, rank < size - 1, rank, &ODD_COMM);
            else
                ODD_COMM = MPI_COMM_WORLD;

            if (pos + 1 != arraySize - 1) {
                // Compare exchange 1-2, 3-4...
                compareExchange(initialArray, pos, 1);

                // Insert compared-exchanged values in initial array for all processes
                MPI_Allgather(initialArray + pos + 1, 2, MPI_INT, initialArray + j * 2 * size + 1, 2, MPI_INT,
                              ODD_COMM);

                // Send whole array from process 0 to last process
                if (rank == 0 && j == nIterationsPerProcess - 1)
                    MPI_Send(initialArray, arraySize, MPI_INT, size - 1, 0, MPI_COMM_WORLD);
            }
                // If process is the last one, receive whole array sent by process 0
            else
                MPI_Recv(initialArray, arraySize, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        }

    }

    if (rank == 0) {
        duration += MPI_Wtime();
        printf("Sorted array: ");
        printArray(initialArray, arraySize);
        printf("Duration: %lfs\n", duration);
    }

    MPI_Finalize();
    return 0;
}