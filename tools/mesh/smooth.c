/**
 *  @file    smooth.c
 *  @author  Nathan Baker
 *  @brief   Convolve grid data with various filters
 *  @version $Id$
 */

/*                                                                                           
 *  Last update: 08/29/2016 by Leighton Wilson                                               
 *  Description: Added ability to read in and output binary DX files                           
 */ 

#include "apbs.h"

#define IJK(i,j,k)  (((k)*(nx)*(ny))+((j)*(nx))+(i))
#define ERRRC 2

typedef enum Smooth_Filter {
    SM_GAUSSIAN  /**< Gaussian filter */
} Smooth_Filter;


VEMBED(rcsid="$Id$")

int gaussian(Vgrid *grid, double stddev, double bandwidth);

int usage(int rc) {

    char *usage = "\n\n\
    ----------------------------------------------------------------------\n\
    This driver program convolves grid data with various filters.  It is\n\
    invoked as:\n\
      smooth <args>\n\n\
    where <args> is a list of the following arguments:\n\
      REQUIRED GENERAL ARGUMENTS:\n\
      --format=<format>  where <format> specifies the data format and is one\n\
                          of the following:\n\
                                dx (standard OpenDX)\n\
                                dxbin (binary OpenDX)\n\
      --input=<file>     where <file> is the input mesh data in the\n\
                         specified format\n\
      --output=<file>    where <file> is the output mesh data in the\n\
                         specified format\n\
      --filter=<filter>  where <filter> is the filter with which the data\n\
                         will be convolved and is one of the following:\n\
                         gaussian (Gaussian filter)\n\
      REQUIRED FILTER-SPECIFIC ARGUMENTS:\n\
        Gaussian filter:\n\
        --stddev=<n>     the standard deviation of the filter (in A)\n\
        --bandwidth=<n>  the bandwith of the filter (in units of stddev)\n\
    ----------------------------------------------------------------------\n\n";

    Vnm_print(2, usage);

    exit(rc);

    return 0;
}

int main(int argc, char **argv) {

    /* *************** VARIABLES ******************* */
    int i;
    Vgrid *grid = VNULL;
    /* Input parameters */
    Vdata_Format format; int gotFormat = 0;
    char inPath[VMAX_BUFSIZE]; int gotInPath = 0;
    char outPath[VMAX_BUFSIZE]; int gotOutPath = 0;
    Smooth_Filter filter; int gotFilter = 0;
    double stddev; int gotStddev = 0;
    double bandwidth; int gotBandwidth = 0;

    char *header = "\n\n\
    ----------------------------------------------------------------------\n\
    ----------------------------------------------------------------------\n\
    \n\n";

    /* *************** CHECK INVOCATION ******************* */
    Vio_start();
    Vnm_redirect(1);
    Vnm_print(1, "%s", header);
    for (i=1; i<argc; i++) {
        Vnm_print(1, "Parsing: %s...\n", argv[i]);
        if (strstr(argv[i], "--format") != NULL) {
            if (strstr(argv[i], "dxbin") != NULL) {
                gotFormat = 1;
                format = VDF_DXBIN;
            } else if (strstr(argv[i], "dx") != NULL) {
                gotFormat = 1;
                format = VDF_DX;
            } else {
                Vnm_print(2, "Error:  %s\n", argv[i]);
                usage(2);
            }
        } else if (strstr(argv[i], "--input") != NULL) {
            if (sscanf(argv[i], "--input=%s", inPath) == 1) gotInPath = 1;
            else {
                Vnm_print(2, "Error:  %s\n", argv[i]);
                usage(2);
            }
        } else if (strstr(argv[i], "--output") != NULL) {
            if (sscanf(argv[i], "--output=%s", outPath) == 1) gotOutPath = 1;
            else {
                Vnm_print(2, "Error:  %s\n", argv[i]);
                usage(2);
            }
        } else if (strstr(argv[i], "--filter") != NULL) {
            if (strstr(argv[i], "gaussian") != NULL) {
                gotFilter = 1;
                filter = SM_GAUSSIAN;
            } else {
                Vnm_print(2, "Error:  %s\n", argv[i]);
                usage(2);
            }
        } else if (strstr(argv[i], "--stddev") != NULL) {
            if (sscanf(argv[i], "--stddev=%lf", &stddev) == 1) gotStddev = 1;
            else {
                Vnm_print(2, "Error:  %s\n", argv[i]);
                usage(2);
            }
        } else if (strstr(argv[i], "--bandwidth") != NULL) {
            if (sscanf(argv[i], "--bandwidth=%lf", &bandwidth) == 1)
                gotBandwidth = 1;
            else {
                Vnm_print(2, "Error:  %s\n", argv[i]);
                usage(2);
            }
        } else {
            Vnm_print(2, "Error:  %s\n", argv[i]);
            usage(2);
        }
    }
    if (!gotFormat) {
        Vnm_print(2, "Error:  --format not specified!\n");
        usage(2);
    }
    if (!gotInPath) {
        Vnm_print(2, "Error:  --input not specified!\n");
        usage(2);
    }
    if (!gotOutPath) {
        Vnm_print(2, "Error:  --output not specified!\n");
        usage(2);
    }
    if (!gotFilter) {
        Vnm_print(2, "Error:  --filter not specified!\n");
        usage(2);
    }
    if (filter == SM_GAUSSIAN) {
        if (!gotStddev) {
            Vnm_print(2, "Error:  --stddev not specified!\n");
            usage(2);
        }
        if (!gotBandwidth) {
            Vnm_print(2, "Error:  --bandwidth not specified!\n");
            usage(2);
        }
    }

    /* *************** READ DATA ******************* */
    Vnm_print(1, "main:  Reading data from %s...\n", inPath);
    grid = Vgrid_ctor(0, 0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, VNULL);
    if (format == VDF_DX) {
        if (!Vgrid_readDX(grid, "FILE", "ASC", VNULL, inPath)) {
            Vnm_print(2, "main:  Problem reading standard OpenDX-format grid from %s\n",
              inPath);
            return ERRRC;
        }
    } else if (format == VDF_DXBIN) {
        if (!Vgrid_readDXBIN(grid, "FILE", "ASC", VNULL, inPath)) {
            Vnm_print(2, "main:  Problem reading binary OpenDX-format grid from %s\n",
              inPath);
            return ERRRC;
        }
    }

    /* *************** SMOOTH ******************* */
    switch(filter) {
        case SM_GAUSSIAN:
           Vnm_print(1, "Smoothing data with Gaussian filter...\n");
           gaussian(grid, stddev, bandwidth);
           break;
        default:
           Vnm_print(2, "Invalid format (%d)!\n", format);
           usage(2);
    }

    /* *************** READ DATA ******************* */
    Vnm_print(1, "main:  Writing data to %s...\n", outPath);
    if (format == VDF_DX)
      Vgrid_writeDX(grid, "FILE", "ASC", VNULL, outPath, "Smoothed data",
        VNULL);
    else if (format == VDF_DXBIN)
      Vgrid_writeDXBIN(grid, "FILE", "ASC", VNULL, outPath, "Smoothed data",
        VNULL);

    return 0;
}

int gaussian(Vgrid *grid, double stddev, double bandwidth) {

    int nx, ny, nz, iband, jband, kband, i, j, k, ii, jj, kk;
    int kkmin, jjmin, iimin, kkmax, jjmax, iimax;
    double hx, hy, hzed, xmin, ymin, zmin, *newData, *oldData, scal, norm;
    double u, ga, dist2;

    Vnm_print(1, "Gaussian filter:  std. dev. = %g A, bandwidth = %g A.\n",
      stddev, bandwidth*stddev);

    nx = grid->nx; ny = grid->ny; nz = grid->nz;
    hx = grid->hx; hy = grid->hy; hzed = grid->hzed;
    xmin = grid->xmin; ymin = grid->ymin; zmin = grid->zmin;
    Vnm_print(1, "Grid:  %d x %d x %d points\n", nx, ny, nz);
    Vnm_print(1, "Grid:  %g, %g, %g A spacing\n", hx, hy, hzed);
    Vnm_print(1, "Grid:  (%g, %g, %g) A origin\n", xmin, ymin, zmin);

    /* Convert HALF bandwidth to grid units */
    iband = (int)(stddev*bandwidth/hx);
    jband = (int)(stddev*bandwidth/hy);
    kband = (int)(stddev*bandwidth/hzed);
    /* Special handling for iband, jband and kband, they are non-zero positive integers */
    if (iband == 0) iband = 1;
    if (jband == 0) jband = 1;
    if (kband == 0) kband = 1;
    Vnm_print(1, "Bandwidth converted to %d x %d x %d grid units.\n",
      iband, jband, kband);
    Vnm_print(1, "This means any non-zero data within (%g, %g, %g) of the\n",
      (iband+1)*hx, (jband+1)*hy, (kband+1)*hzed);
    Vnm_print(1, "domain boundary will be convolved differently.\n");

    /* Get exponent scaling factor */
    scal = 2.0 * stddev * stddev;
    VASSERT(scal > 0);
    scal = 1.0/scal;

    /* Get data */
    oldData = grid->data;
    newData = Vmem_malloc(VNULL, (nx*ny*nz), sizeof(double));

    /* Copy over old data.  All but the boundary values will be replaced in the next step so this is more copying than is strictly
       necessary... */
    for (i=0; i<(nx*ny*nz); i++) newData[i] = oldData[i];

    /* Apply filter */
    for (k=1; k<(nz-1); k++) {
        kkmin = VMAX2(1, (k - kband));
        kkmax = VMIN2((nz-1), (k + kband));
        for (j=1; j<(ny-1); j++) {
            jjmin = VMAX2(1, (j - jband));
            jjmax = VMIN2((ny-1), (j + jband));
            for (i=1; i<(nx-1); i++) {
                iimin = VMAX2(1, (i - iband));
                iimax = VMIN2((nx-1), (i + iband));
                u = 0;
                /* We normalize within the loop to conserve densities */
                norm = 0;
                for (kk=kkmin; kk<kkmax; kk++) {
                    for (jj=jjmin; jj<jjmax; jj++) {
                        for (ii=iimin; ii<iimax; ii++) {
                            dist2 = VSQR(hx*(i-ii)) + VSQR(hy*(j-jj)) +
                              VSQR(hzed*(k-kk));
                            ga = VEXP(-dist2*scal);
                            u += (ga*oldData[IJK(ii,jj,kk)]);
                            norm += ga;
                        } /* ii loop */
                    } /* jj loop */
                } /* kk loop */
                if (norm > 0) newData[IJK(i,j,k)] = u/norm;
                else newData[IJK(i,j,k)] = 0;
            } /* i loop */
        } /* j loop */
    } /* k loop */

    /* Replace data */
    for (i=0; i<(nx*ny*nz); i++) grid->data[i] = newData[i];
    Vmem_free(VNULL, (nx*ny*nz), sizeof(double), (void **)&newData);

    return 0;
}
