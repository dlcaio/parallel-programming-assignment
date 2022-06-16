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

void populateArray(int *array, int n) {
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        array[i] = rand() % 100;
    }
}

int isCorrect(int *array, int* sane, int nSane) {
  for (int i = 0; i < nSane; i++) {
    if (array[i] != sane[i]) return 0;
  }
  return 1;
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
        populateArray(saneArray, arraySize);
        bubbleSort(saneArray, arraySize);
        printf("Array to be sorted: ");
        printArray(initialArray, arraySize);
        duration -= MPI_Wtime();
    }
    MPI_Bcast(initialArray, arraySize, MPI_INT, 0, MPI_COMM_WORLD);

    int nIterationsPerProcess = arraySize / (size * 2);

    // One even and one odd iteration for each i in arraySize
    for (int i = 0; i < arraySize / 2; i++) {
        for (int j = 0; j < nIterationsPerProcess; j++) {
            // int pos = (rank * 2) + j * 2 * size;

            // compareExchange(initialArray, pos, 0);
            // 

            int pos = (rank * 2) * nIterationsPerProcess + 2 * j;
            compareExchange(initialArray, pos, 0);

            // p0: 0  rank * nIterationsPerProcess
            //   : 2  (rank * 2) * nIterationsPerProcess
            // p1: 4  (rank * 2) * nIterationsPerProcess
            //   : 6  (rank * 2) * nIterationsPerProcess + 2
            // p2: 8
            //   : 10
        }

        MPI_Allgather(initialArray + rank * 2 * nIterationsPerProcess, 2 * nIterationsPerProcess, MPI_INT, initialArray,  2 * nIterationsPerProcess, MPI_INT, MPI_COMM_WORLD);

        for (int j = 0; j < nIterationsPerProcess; j++) {
            int pos = (rank * 2) * nIterationsPerProcess + 2 * j;

            if (pos + 1 != arraySize - 1) {
              compareExchange(initialArray, pos, 1);
            } 
          }
          int s = 2;
          if (rank == size - 1) s = 1;

          int counts[4] = {2, 2, 2, 1};
          int displacements[4] = {1, 3, 5, 7};

          MPI_Allgatherv(initialArray + rank * 2 * nIterationsPerProcess + 1, s, MPI_INT, initialArray, counts, displacements, MPI_INT, MPI_COMM_WORLD);

          //p0 = 1
          //p1 = 3
          //p2 = 5
          //p3 = 7

            /*
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
            */
        
    }

    if (rank == 0) {
        duration += MPI_Wtime();
        printf("Sorted array: ");
        printArray(initialArray, arraySize);
        printf("Duration: %lfs\n", duration);
        printf("Is correct? %d\n", isCorrect(initialArray, saneArray, arraySize));
    }

    MPI_Finalize();
    return 0;
}