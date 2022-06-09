#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define ARRAY_SIZE 16

void printArray(int *array, int n)
{
  printf("[ ");
  for (int i = 0; i < n; i++)
  {
    printf("%d ", array[i]);
  }
  printf("]\n");
}

void compareExchange(int *array, int pos, int odd)
{
  int newPos = pos + odd;
  if (array[newPos] > array[newPos + 1])
  {
    int temp = array[newPos];
    array[newPos] = array[newPos + 1];
    array[newPos + 1] = temp;
  }
}

int main(int argc, char **argv)
{
  int rank, size;
  int initial_array[ARRAY_SIZE] = {9, 4, 2, 1, 5, 3, 6, 10, 19, 14, 12, 11, 15, 110, 13, 16};

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // One even and one odd iteration for each i in ARRAY_SIZE
  for (int i = 0; i < ARRAY_SIZE; i++)
  {
    MPI_Barrier(MPI_COMM_WORLD);

    for (int j = 0; j < 2; j++)
    {

      int pos = (rank * 2) + j * 8;
      compareExchange(initial_array, pos, 0);
      MPI_Barrier(MPI_COMM_WORLD);
      // printf("process %d, i = %d, j = %d arr[%d, %d] is [%d, %d]\n", rank, i, j, pos, pos + 1, initial_array[pos], initial_array[pos + 1]);

      MPI_Barrier(MPI_COMM_WORLD);

      MPI_Allgather(initial_array + pos, 2, MPI_INT, initial_array + j * 8, 2, MPI_INT, MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    for (int j = 0; j < 2; j++)

    {

      int pos = (rank * 2) + j * 8;

      MPI_Comm ODD_COMM;

      if (j == 1)
      {
        MPI_Comm_split(MPI_COMM_WORLD, rank < 3, rank, &ODD_COMM);
      }
      else
      {
        ODD_COMM = MPI_COMM_WORLD;
      }

      MPI_Barrier(MPI_COMM_WORLD);

      if (pos + 1 != ARRAY_SIZE - 1)
      {
        // Compare exchange 1-2, 3-4...
        compareExchange(initial_array, pos, 1);

        // Insert compared-exchanged values in initial array for all processes
        MPI_Allgather(initial_array + pos + 1, 2, MPI_INT, initial_array + j * 8 + 1, 2, MPI_INT, ODD_COMM);

        // Send whole array from process 0 to last process
        if (rank == 0 && j == 1)
          MPI_Send(initial_array, ARRAY_SIZE, MPI_INT, size - 1, 0, MPI_COMM_WORLD);
      }
      // If process is the last one, receive whole array sent by process 0
      else
      {
        
        MPI_Recv(initial_array, ARRAY_SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    printArray(initial_array, 16);

    MPI_Barrier(MPI_COMM_WORLD);
  }
  MPI_Barrier(MPI_COMM_WORLD);
  if (rank == 0)
    printArray(initial_array, ARRAY_SIZE);

  MPI_Finalize();
  return 0;
}