#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <time.h>

void bubbleSort(int *array, int n)
{
  int temp;
  for (int k = 1; k < n; k++)
  {
    for (int j = 0; j < n - k; j++)
    {
      if (array[j] > array[j + 1])
      {
        temp = array[j];
        array[j] = array[j + 1];
        array[j + 1] = temp;
      }
    }
  }
}

void populateArray(int *array, int n)
{
  srand(time(NULL));
  for (int i = 0; i < n; i++)
  {
    array[i] = rand() % 100;
  }
}

int isSanityChecked(int *array, int *sane, int n)
{
  for (int i = 0; i < n; i++)
  {
    if (array[i] != sane[i])
      return 0;
  }
  return 1;
}

int isSorted(int *array, int n)
{
  int prevElem = array[0];
  for (int i = 1; i < n; i++)
  {
    if (prevElem > array[i])
    {
      return 0;
    }
    prevElem = array[i];
  }
  return 1;
}

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
  int arraySize = atoi(argv[1]);
  int initialArray[arraySize];
  int saneArray[arraySize];
  double duration = 0.0;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (rank == 0)
  {
    printf("Parallel OddEven GeeksForGeeks Version\n");
    populateArray(initialArray, arraySize);
    populateArray(saneArray, arraySize);
    bubbleSort(saneArray, arraySize);
    // printf("Array to be sorted: ");
    // printArray(initialArray, arraySize);
    duration -= MPI_Wtime();
  }
  MPI_Bcast(initialArray, arraySize, MPI_INT, 0, MPI_COMM_WORLD);

  int displacements[size], counts[size];

  for (int i = 0; i < size; i++)
  {
    displacements[i] = (arraySize / size) * i + 1;
    counts[i] = arraySize / size - (i == size - 1);
  }

  int nIterationsPerProcess = arraySize / (size * 2);

  // One even and one odd iteration for each i in arraySize
  for (int i = 0; i < arraySize / 2; i++)
  {
    int pos;

    int eachProccessChunkSize = 2 * nIterationsPerProcess;

    int startingPos = rank * eachProccessChunkSize;

    for (int j = 0; j < eachProccessChunkSize; j += 2)
    {
      pos = startingPos + j;
      compareExchange(initialArray, pos, 0);
    }

    MPI_Allgather(initialArray + startingPos, eachProccessChunkSize, MPI_INT, initialArray, eachProccessChunkSize, MPI_INT, MPI_COMM_WORLD);

    for (int j = 0; j < eachProccessChunkSize; j += 2)
    {
      pos = startingPos + j;
      if (pos + 1 != arraySize - 1)
        compareExchange(initialArray, pos, 1);
    }

    MPI_Allgatherv(initialArray + startingPos + 1, counts[rank], MPI_INT, initialArray, counts, displacements, MPI_INT, MPI_COMM_WORLD);
  }

  if (rank == 0)
  {
    duration += MPI_Wtime();
    // printf("Sorted array: ");
    // printArray(initialArray, arraySize);
    printf("Duration: %lfs\n", duration);
    printf("Is it correct? %d\n", isSanityChecked(initialArray, saneArray, arraySize));
  }

  MPI_Finalize();
  return 0;
}