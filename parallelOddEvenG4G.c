#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

void printArray(int *array, int n)
{
  printf("[ ");
  for (int i = 0; i < n; i++)
  {
    printf("%d ", array[i]);
  }
  printf("]\n");
}

int main(int argc, char **argv)
{
  int rank, size;
  int *inner_array = malloc(sizeof(int) * 16);
  int initial_array[8] = {2, 1, 9, 4, 5, 3, 6, 10};
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  for (int i = 0; i < 8; i++)
  {
    int temp[2];
    int firstElement = initial_array[rank * 2];
    int secondElement = initial_array[rank * 2 + 1];

    if (secondElement < firstElement)
    {
      temp[0] = secondElement;
      temp[1] = firstElement;
    }
    else
    {
      temp[0] = firstElement;
      temp[1] = secondElement;
    }

    // printf("Process number %d, analysing number [%d, %d]... %d is greater than %d\n", rank, firstElement, secondElement, temp[1], temp[0]);

    // if (rank == 0)

    //  printArray(initial_array, 8);

    MPI_Barrier(MPI_COMM_WORLD);

    memcpy(initial_array + rank * 2, temp, 2 * sizeof(int));

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Allgather(initial_array + rank * 2, 2, MPI_INT, initial_array, 2, MPI_INT, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Comm new_comm;
    int color = (rank < 3);
    MPI_Comm_split(MPI_COMM_WORLD, color, rank, &new_comm);

    if ((rank * 2 + 1) != 7)
    {
      int firstElement = initial_array[rank * 2 + 1];
      int secondElement = initial_array[rank * 2 + 2];

      if (secondElement < firstElement)
      {
        temp[0] = secondElement;
        temp[1] = firstElement;
      }
      else
      {
        temp[0] = firstElement;
        temp[1] = secondElement;
      }
      MPI_Barrier(new_comm);
      printf("Process number %d, analysing number [%d, %d]... %d is greater than %d\n", rank, firstElement, secondElement, temp[1], temp[0]);
      MPI_Barrier(new_comm);
      
      memcpy(initial_array + rank * 2 + 1, temp, 2 * sizeof(int));
      printArray(initial_array, 8);
      MPI_Barrier(new_comm);
      MPI_Allgather(initial_array + rank * 2 + 1, 2, MPI_INT, initial_array + 1, 2, MPI_INT, new_comm);
      printArray(initial_array, 8);
      MPI_Barrier(new_comm);
      if (rank == 0) MPI_Send(initial_array, 8, MPI_INT, size - 1, 0, MPI_COMM_WORLD);
    } else {
      MPI_Recv(initial_array, 8, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
  }

  free(inner_array);
  MPI_Finalize();
  return 0;
}