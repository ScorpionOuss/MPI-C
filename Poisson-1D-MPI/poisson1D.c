/* Reaction-diffusion equation in 1D
 * Solution by Jacobi iteration
 * simple MPI implementation
 *
 * C Michael Hanke 2006-12-12
 */


#define MIN(a,b) ((a) < (b) ? (a) : (b))

/* Use MPI */
#include "mpi.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h> 
#include <math.h>

/* define problem to be solved */
#define N 1000   /* number of inner grid points */
#define SMX 1000000 /* number of iterations */

/* implement coefficient functions */
extern double r(const double x);
extern double f(const double x);

/* We assume linear data distribution. The formulae according to the lecture
   are:
      L = N/P;
      R = N%P;
      I = (N+P-p-1)/P;    (number of local elements)
      n = p*L+MIN(p,R)+i; (global index for given (p,i)
   Attention: We use a small trick for introducing the boundary conditions:
      - The first ghost point on p = 0 holds u(0);
      - the last ghost point on p = P-1 holds u(1).
   Hence, all local vectors hold I elements while u has I+2 elements.
*/



int main(int argc, char *argv[])
{
/********************************Initialization*********************************/ 
/* local variable */
    int p, P, tag, rc; 
/* Initialize MPI */
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &P);
    MPI_Comm_rank(MPI_COMM_WORLD, &p);
    MPI_Status status;
    if (N < P) {
	fprintf(stdout, "Too few discretization points...\n");
	exit(1);
    }
    tag = 10;
/* Compute local indices for data distribution */
    int L = N/P;
    int R = N%P;
    int I = (N + P -p - 1)/P;
    double h = ((double)1.0) / (N + 1);


/* arrays */
    double *unew = (double *) calloc(I, sizeof(double));

/* Note: The following allocation includes additionally:
   - boundary conditions are set to zero
   - the initial guess is set to zero */
    double *u = (double *) calloc(I+2, sizeof(double));


    double ff[I], rr[I];
    /* Initialize rr and ff */
    for (int i = 0; i < I; i++)
    {
        int cv = p*L + MIN(p,R) + i;
        double xn = cv * h;
        ff[i] = f(xn);
        rr[i] = r(xn);
        u[i] = 0;
        unew[i] = 0;
    }

    u[I] = 0;
    u[I + 1] = 0;

    /* Deal with limit cases */
    int succ, pred;
    if (p == P - 1){
        succ = MPI_PROC_NULL;
    } else{
        succ = p + 1;
    }

    if (p == 0){
        pred = MPI_PROC_NULL;
    } else{
        pred = p - 1;
    }
    /******************************************Computations************************************/
/* Jacobi iteration */
    for (int step = 0; step < SMX; step++) {
	    /* Local iteration */
        for (int i = 0; i < I; i++){
            unew[i] = (u[i]+u[i+2]-h*h*ff[i])/(2.0-h*h*rr[i]);
        }
        /** Red/black communication **/
        
	if (p%2 == 0){
            // Deal with special cases
            rc = MPI_Send(&unew[0], 1, MPI_DOUBLE, pred, tag, MPI_COMM_WORLD);
            rc = MPI_Recv(&u[0], 1, MPI_DOUBLE, pred, tag, MPI_COMM_WORLD, &status);
            rc = MPI_Send(&unew[I - 1], 1, MPI_DOUBLE, succ, tag, MPI_COMM_WORLD);           
            rc = MPI_Recv(&u[I + 1], 1, MPI_DOUBLE, succ, tag, MPI_COMM_WORLD, &status);
        } else{
            rc = MPI_Recv(&u[I + 1], 1, MPI_DOUBLE, succ, tag, MPI_COMM_WORLD, &status);
            rc = MPI_Send(&unew[I - 1], 1, MPI_DOUBLE, succ, tag, MPI_COMM_WORLD);
            rc = MPI_Recv(&u[0], 1, MPI_DOUBLE, pred, tag, MPI_COMM_WORLD, &status);
            rc = MPI_Send(&unew[0], 1, MPI_DOUBLE, pred, tag, MPI_COMM_WORLD);
            
        }
        /* Memory swap */
        memcpy(u + 1, unew, I*sizeof(double));
    }

    /*************************************Output**************************************************/
/* output for graphical representation */
/* Instead of using gather (which may lead to excessive memory requirements
   on the master process) each process will write its own data portion. This
   introduces a sequentialization: the hard disk can only write (efficiently)
   sequentially. Therefore, we use the following strategy:
   1. The master process writes its portion. (file creation)
   2. The master sends a signal to process 1 to start writing.
   3. Process p waites for the signal from process p-1 to arrive.
   4. Process p writes its portion to disk. (append to file)
   5. process p sends the signal to process p+1 (if it exists).
*/
    if (p > 0){
	MPI_Recv(0, 0, MPI_CHAR, pred, tag, MPI_COMM_WORLD, &status);
	}
    
    FILE *fp = fopen("out.b", "a");
    for (int i = 0; i < I; i++){
	fprintf(fp, "%f ", unew[i]);
    }

    fclose(fp);
    
    MPI_Send(0, 0, MPI_CHAR, succ, tag, MPI_COMM_WORLD);

	
    printf("Process %d End \n", p);
/* That's it */
    MPI_Finalize();
    exit(0);
}

double r(const double x){
    /**
     * @brief
     * 
     */
    return x + 1.0;
}

double f(const double x){
    /**
     * @brief 
     * 
     */
    return (double) sin(25*M_PI*x);
}
