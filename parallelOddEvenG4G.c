#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define ARRAY_SIZE 8

void printArray(int *array, int n)
{
  printf("[ ");
  for (int i = 0; i < n; i++)
  {
    printf("%d ", array[i]);
  }
  printf("]\n");
}

void compareExchange(int *array, int rank, int odd)
{
  int pos = rank * 2 + odd;
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
  int initial_array[ARRAY_SIZE] = {9, 4, 2, 1, 5, 3, 6, 10};

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // One even and one odd iteration for each i in ARRAY_SIZE
  for (int i = 0; i < ARRAY_SIZE; i++)
  {

    // Even iteration

    // Compare exchange 0-1, 2-3...
    compareExchange(initial_array, rank, 0);

    // Insert compared-exchanged values in initial array for all processes
    MPI_Allgather(initial_array + rank * 2, 2, MPI_INT, initial_array, 2, MPI_INT, MPI_COMM_WORLD);

    // Odd iteration

    // Create communicator for odd iteration, since last process won't do work in this iteration
    MPI_Comm ODD_COMM;
    MPI_Comm_split(MPI_COMM_WORLD, (rank < 3), rank, &ODD_COMM);

    // Check if process is the last one
    if ((rank * 2 + 1) != ARRAY_SIZE - 1)
    {
      // Compare exchange 1-2, 3-4...
      compareExchange(initial_array, rank, 1);
      
      // Insert compared-exchanged values in initial array for all processes
      MPI_Allgather(initial_array + rank * 2 + 1, 2, MPI_INT, initial_array + 1, 2, MPI_INT, ODD_COMM);
      
      // Send whole array from process 0 to last process
      if (rank == 0)
        MPI_Send(initial_array, ARRAY_SIZE, MPI_INT, size - 1, 0, MPI_COMM_WORLD);
    }
    // If process is the last one, receive whole array sent by process 0
    else
    {
      MPI_Recv(initial_array, ARRAY_SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
  }

  if (rank == 0) printArray(initial_array, ARRAY_SIZE);

  MPI_Finalize();
  return 0;
}