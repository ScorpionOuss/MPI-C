/*** function that returns the color ***/

#include <complex.h>
#include <stdio.h>
#include <stdlib.h>

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

void main(){
    /**
     * @brief The main function takes the number of pixels and does the
     * mapping with the interval [-b, b]*[-b, b]
     * Then for each pixel, it computes the color and finally calls
     * display to print the graphical representation
     * 
     */
    float dx = ((float) 2*b)/(w-1);
    float dy = ((float) 2*b)/(h-1);
    double complex d;
    float dreal, dimag;
    unsigned char *color = malloc(w*h*sizeof(unsigned char));
    for (int y = 0; y < h; y++){
        dimag = y*dy - b;
         for (int x = 0; x < w; x++){
             dreal = x*dx - b;
             d = dreal + dimag * I;
             printf("%f, %f\n", dreal, dimag);
             color[x + y*w] = cal_pixel(d, b, N);
         }
    }

    FILE *fp;
    /* Printing the computations in ascii format*/
    fp = fopen("color.txt", "w");
    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            fprintf(fp, "%hhu ", color[i + j*w]);
        }
        fprintf(fp, "\n");
        }
        fclose(fp);
}
