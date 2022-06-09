#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define ARRAY_SIZE 32

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
  int initial_array[ARRAY_SIZE] = {9, 4, 120, 1, 5, 50, 6, -10, 19, 14, 12, 11, 15, 110, 13, 16, -9, 40, 1231, 12, 55, -50, 6666, -102, 194, 140, 12, 11, -15, -110, 132, 16};
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int nIterationsPerProcess = ARRAY_SIZE / (size * 2);

  // One even and one odd iteration for each i in ARRAY_SIZE
  for (int i = 0; i < ARRAY_SIZE; i++)
  {
    int pos;
    for (int j = 0; j < nIterationsPerProcess; j++)
    {
      pos = (rank * 2) + j * 2 * size;
      compareExchange(initial_array, pos, 0);
      MPI_Allgather(initial_array + pos, 2, MPI_INT, initial_array + j * 2 * size, 2, MPI_INT, MPI_COMM_WORLD);
    }

    for (int j = 0; j < nIterationsPerProcess; j++)
    {
      pos = (rank * 2) + j * 2 * size;

      MPI_Comm ODD_COMM;

      if (j == nIterationsPerProcess - 1)
        MPI_Comm_split(MPI_COMM_WORLD, rank < size - 1, rank, &ODD_COMM);
      else
        ODD_COMM = MPI_COMM_WORLD;

      MPI_Barrier(MPI_COMM_WORLD);

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