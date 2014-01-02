#include <stdio.h>

#include "apbs.h"

#define MXGRD 129

int main(int argc, char **argv) {

    int im, jm, km, grdflg, one, kk;
    double h, ox, oy, oz;
    double scale;
    double dum2, dum3, dum4, dum5, dum6, dum7, dum8;
    int idum2, idum3, idum4;
    int i, j, k;

    char newfile[256];
    char flnm[256];
    char title[72];

    FILE *inFile = NULL;
    FILE *outFile = NULL;

    double grid[MXGRD * MXGRD * MXGRD];
    MAT3(grid, MXGRD, MXGRD, MXGRD);

    printf("Convert UHBD ascii grid to binary\n");
    printf("---------------------------------\n");

    printf("Enter grid map file name: ");
    scanf("%s", flnm);

    inFile = fopen(flnm, "r");
    if( inFile == NULL )
    {
        printf("Couldn't open output file: %s\n", flnm);
        return -1;
    }



    printf("Enter output file name: ");
    scanf("%s", newfile);

    outFile = fopen(newfile, "wb");
    if( outFile == NULL )
    {
        printf("Couldn't open output file: %s\n", newfile);
        return -2;
    }


    /* Read in the DX regular positions */
    /* Get "object" */
    fscanf(inFile, "%s", title);
    fwrite(title, 1, 72, outFile);

    fscanf(inFile, "%lf", &scale);
    fwrite(&scale, 1, sizeof(double), outFile);

    fscanf(inFile, "%lf", &dum2);
    fwrite(&dum2, 1, sizeof(double), outFile);

    fscanf(inFile, "%d", &grdflg);
    fwrite(&grdflg, 1, sizeof(int), outFile);

    fscanf(inFile, "%d", &idum2);
    fwrite(&idum2, 1, sizeof(int), outFile);

    fscanf(inFile, "%d", &km);
    fwrite(&km, 1, sizeof(int), outFile);

    fscanf(inFile, "%d", &one);
    fwrite(&one, 1, sizeof(int), outFile);

    fscanf(inFile, "%d", &km);
    fwrite(&km, 1, sizeof(int), outFile);

    fscanf(inFile, "%d", &im);
    fwrite(&im, 1, sizeof(int), outFile);

    fscanf(inFile, "%d", &jm);
    fwrite(&jm, 1, sizeof(int), outFile);

    fscanf(inFile, "%d", &km);
    fwrite(&km, 1, sizeof(int), outFile);

    fscanf(inFile, "%lf", &h);
    fwrite(&h, 1, sizeof(double), outFile);

    fscanf(inFile, "%lf", &ox);
    fwrite(&ox, 1, sizeof(double), outFile);

    fscanf(inFile, "%lf", &oy);
    fwrite(&oy, 1, sizeof(double), outFile);

    fscanf(inFile, "%lf", &oz);
    fwrite(&oz, 1, sizeof(double), outFile);

    fscanf(inFile, "%lf", &dum3);
    fwrite(&dum3, 1, sizeof(double), outFile);

    fscanf(inFile, "%lf", &dum4);
    fwrite(&dum4, 1, sizeof(double), outFile);

    fscanf(inFile, "%lf", &dum5);
    fwrite(&dum5, 1, sizeof(double), outFile);

    fscanf(inFile, "%lf", &dum6);
    fwrite(&dum6, 1, sizeof(double), outFile);

    fscanf(inFile, "%lf", &dum7);
    fwrite(&dum7, 1, sizeof(double), outFile);

    fscanf(inFile, "%lf", &dum8);
    fwrite(&dum8, 1, sizeof(double), outFile);

    fscanf(inFile, "%d", &idum3);
    fwrite(&idum3, 1, sizeof(int), outFile);

    fscanf(inFile, "%d", &idum4);
    fwrite(&idum4, 1, sizeof(int), outFile);

    for (k=0; k<km; k++) {

        fscanf(inFile, "%d", &kk);
        fwrite(&kk, 1, sizeof(int), outFile);

        fscanf(inFile, "%d", &im);
        fwrite(&im, 1, sizeof(int), outFile);

        fscanf(inFile, "%d", &jm);
        fwrite(&jm, 1, sizeof(int), outFile);

        for (j=0; j<jm; j++) {

            for (i=0; i<im; i++ ) {

                fscanf(inFile, "%lf", RAT3(grid, i, j, k));
                fwrite(RAT3(grid, i, j, k), 1, sizeof(double), outFile);

            }
        }
    }

    return 0;
}
