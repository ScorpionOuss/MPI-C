/* sample hello world program  *
 *  C Michael Hanke 2006-10-19 */

#include <stdio.h>
#include <string.h>
#include <mpi.h>

main(int argc, char **argv)
{
  int rank, size, tag, rc, i, recvRank;
  MPI_Status status;
  char message[20];

  rc = MPI_Init(&argc, &argv);
  rc = MPI_Comm_size(MPI_COMM_WORLD, &size);
  rc = MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  tag = 100;
  if (rank == 0) {
    strcpy(message, "Hello, world");
    for (i = 1; i < size; i++)
      rc = MPI_Send(message, 13, MPI_CHAR, i, tag, MPI_COMM_WORLD);
    
    for (i = 1; i < size; i++){
      rc = MPI_Recv(&recvRank, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &status);
      printf("I received ack from node n° %d\n", recvRank);
    }
  }
  else {
    rc = MPI_Recv(message, 13, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status);
    rc = MPI_Send(&rank, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
  }
  printf("node %d : %.13s\n", rank, message);
  rc = MPI_Finalize();
}