/*** Implementation SPMD ***/

#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

const int N = 256; // max Iterations
const int b = 2; // Bound of divergence
const int w = 2048; // Width
const int h = 2048; // Height

unsigned char cal_pixel(double complex comp, int bound, int maxIter){
    /**
     * @brief Computes the index from which the sequence diverge
     * 
     */
    unsigned int count = 1;
    double complex z = 0;
    while((cabs(z) < bound) && (count < maxIter)){
        z = z*z + comp;
        count++;
    }
    return (unsigned char) count;
}

void main(int argc, char** argv){
    /**
     * @brief The main function takes the number of pixels and does the
     * mapping with the interval [-b, b]*[-b, b]
     * Then for each pixel, it computes the color and finally calls
     * display to print the graphical representation
     * 
     */

    // Initialize MPI
    int rank, numProc, rc, tag;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numProc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Status status;
    tag = 10;

    float dx = ((float) 2*b)/(w-1);
    float dy = ((float) 2*b)/(h-1);
    /*** We divide the problem by dividing the width ***/
    int wp = w/numProc; /**We should verify that the w can be devided by nP*/

    int xoff = rank*wp;
    /* yoff = 0 */
    double complex d;
    float dreal, dimag;
    unsigned char color[wp][h];
    for (int y = 0; y < h; y++){
        dimag = y*dy - b;
        for (int x = 0; x < wp; x++){
            dreal = (x + xoff)*dx - b;
            d = dreal + dimag * I;
            color[x][y] = cal_pixel(d, b, N);
        }
    }

    if (rank == 0){
        /** Declare the global datastructure colors **/
        unsigned char colors[w][h];
        /** First print its own computations**/
        for (int y = 0; y < h; y++){
            for (int x = 0; x < wp; x++){
                colors[x][y] = color[x][y];
            }
        }
        /** Gathering the computations from the other processes **/
        for (int process = 1; process < numProc; process++){
            rc = MPI_Recv(color, sizeof(color), MPI_CHAR, process, tag, MPI_COMM_WORLD, &status);
            for (int y = 0; y < h; y++){
                for (int x = 0; x < wp; x++){
                    colors[x + process*wp][y] = color[x][y];
                }
            }
        }


        FILE *fp;
        /* Printing the computations in ascii format*/
        fp = fopen("color.txt", "w");
        for (int j = 0; j < h; j++) {
            for (int i = 0; i < w; i++) {
                fprintf(fp, "%hhu ", colors[i][j]);
            }
            fprintf(fp, "\n");
            }
            fclose(fp);
    }

    else{
        rc = MPI_Send(color, sizeof(color), MPI_CHAR, 0, tag, MPI_COMM_WORLD);
    }
    rc = MPI_Finalize();

