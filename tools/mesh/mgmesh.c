/**
 *  @file    mgmesh.c
 *  @author  Nathan Baker
 *  @brief   Small program to write out acceptable combinations of grid
 *           dimensions, multigrid levels, etc. for PMG
 */

#include "math.h"

#include "apbs.h"


int main(int argc, char **argv) {

    int i, lev;
    int maxvert = 700;
    int minlev = VMGNLEV;
    double newval, oldval;

    Vio_start();

    Vnm_print(1, "\n\nThis program determines the acceptable meshpoint number\n"
                 "and level combinations for the PMG multigrid libraries and\n"
                 "%d or more levels in the mesh (because you typically use\n"
                 "one less than the max number of levels)\n\n\n", minlev);

    for (i=2; i<maxvert; i++) {
        /* the number of times it's divisible. */
        lev = 0;
        newval = (double)(i-1);
        oldval = (double)(i-1);
        while (1) {
           oldval = newval;
           newval = newval/2.0;
           if ((floor(newval) != newval) || (ceil(newval) != newval)) break;
           lev++;
        }
        if (lev >= minlev) {
            Vnm_print(1, "%4d verts/direction => %d levels\n", i, lev);
            Vnm_print(1, "                        %d verts on coarsest level\n",
              (int)oldval);
            Vnm_print(1, "                        ~%g MB memory (for %d^3 mesh)\n",
              (double)(i*i*i)*160.0/1024.0/1024.0, i);
        }
    }

#if 0
    int i, maxmult = 30;
    int j, maxlev = 5;
    double log2 = log(2.0);
    double x;

    for (i=0; i<maxlev; i++) {
        for (j=0; j<maxmult; j++) {
            printf("%g ", j*pow(2,i+1) + 1);
        }
        printf("\n");
    }
#endif

    return 0;
}
