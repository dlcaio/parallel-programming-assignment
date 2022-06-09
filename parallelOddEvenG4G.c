#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define ARRAY_SIZE 128

void printArray(int *array, int n)
{
  printf("[ ");
  for (int i = 0; i < n; i++)
  {
    printf("%d ", array[i]);
  }
  printf("]\n");
}

void compareExchange(int *array, int posWithoutOdd, int odd)
{
  int pos = posWithoutOdd + odd;
  if (array[pos] > array[pos + 1])
  {
    int temp = array[pos];
    array[pos] = array[pos + 1];
    array[pos + 1] = temp;
  }
}

int main(int argc, char **argv)
{
  int rank, size;
  int initial_array[ARRAY_SIZE] = {-61, -33, -83, -31, -1, 95, -72, -62, 94, 38, -70, 6, 74, -100, 61, 56, -5, -67, -56, 79, -50, 64, 9, 9, -85, 33, -54, 10, 66, 39, 18, 69, -78, -39, 15, -68, 64, -86, 63, 24, 26, -26, 47, 15, -35, -2, 37, 60, 23, -33, -76, -90, -17, 46, -46, 69, 54, 20, 72, -37, -44, -10, 8, 80, -74, 7, -12, -80, -78, -97, -87, 80, -70, 74, 45, 12, -8, 46, -35, 62, -69, 4, -50, -1, -100, -33, 0, -58, -30, -53, 85, -54, -2, 31, -62, -91, -40, 59, -24, -53, 33, 62, -72, 39, -72, 70, 92, -46, -72, 92, 46, -26, -2, 53, -33, 18, 56, 73, 59, -69, -76, 87, 32, -68, -28, -55, -55, -66};
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int nIterationsPerProcess = ARRAY_SIZE / (size * 2);

  // One even and one odd iteration for each i in ARRAY_SIZE
  for (int i = 0; i < ARRAY_SIZE; i++)
  {
    for (int j = 0; j < nIterationsPerProcess; j++)
    {
      int pos = (rank * 2) + j * 2 * size;

      compareExchange(initial_array, pos, 0);
      MPI_Allgather(initial_array + pos, 2, MPI_INT, initial_array + j * 2 * size, 2, MPI_INT, MPI_COMM_WORLD);

      MPI_Comm ODD_COMM;
      if (j == nIterationsPerProcess - 1)
        MPI_Comm_split(MPI_COMM_WORLD, rank < size - 1, rank, &ODD_COMM);
      else
        ODD_COMM = MPI_COMM_WORLD;

      if (pos + 1 != ARRAY_SIZE - 1)
      {
        // Compare exchange 1-2, 3-4...
        compareExchange(initial_array, pos, 1);

        // Insert compared-exchanged values in initial array for all processes
        MPI_Allgather(initial_array + pos + 1, 2, MPI_INT, initial_array + j * 2 * size + 1, 2, MPI_INT, ODD_COMM);

        // Send whole array from process 0 to last process
        if (rank == 0 && j == nIterationsPerProcess - 1)
          MPI_Send(initial_array, ARRAY_SIZE, MPI_INT, size - 1, 0, MPI_COMM_WORLD);
      }
      // If process is the last one, receive whole array sent by process 0
      else
        MPI_Recv(initial_array, ARRAY_SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
  }

  if (rank == 0)
    printArray(initial_array, ARRAY_SIZE);

  MPI_Finalize();
  return 0;
}