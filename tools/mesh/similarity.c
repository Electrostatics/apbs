/**
 *  @file    similarity.c
 *  @author  Nathan Baker
 *  @brief   Program that computes similarity indices for two scalar fields
 *  @version $Id$
 */

#include "apbs.h"

VEMBED(rcsid="$Id$")

/**
 * @brief  Prints usage information and exits
 * @author  Nathan Baker
 * @param  rc  Exit code */
void usage(int rc) {
    char *usage = "\n\
A program to calculate similarity metrics between scalar data sets.\n\
  Usage:  similarity <req args> [opts]\n\
where <req args> are the required arguments:\n\
  --format=<format>  The input file format.  Acceptable values include\n\
       dx  OpenDX format\n\
  --scalar1=<path>  The path to the first scalar data file\n\
  --scalar2=<path>  The path to the second scalar data file\n\
and where [opts] are the options:\n\
  --help  Print this message\n\
  --mask1=<path>  A file with scalar values specifying a \"mask\" or\n\
    characteristic function for the similarity calculation.  This file\n\
    contains values between 1 and 0 which are multiplied against the\n\
    scalar data set 1 before the similarity calculation is performed.\n\
  --mask2=<path>  A file with scalar values specifying a \"mask\" or\n\
    characteristic function for the similarity calculation.  This file\n\
    contains values between 1 and 0 which are multiplied against the\n\
    scalar data set 1 before the similarity calculation is performed.\n\
  --transform=<path>  The path to a file containing the coordinate\n\
    transformation to place data set 2 in the coordinate frame of data set\n\
    1.  The format of this file is:\n\
       a11 a12 a13\n\
       a21 a22 a23\n\
       a31 a32 a33\n\
       b1  b2  b3\n\
    where aij are the elements of a rotation matrix A and bi are the\n\
    elements of a displacement vector b.  The transformed coordinates (y) of\n\
    data set 2 are obtained from the original coordinates (x) by:\n\
       y = A*x + v\n\
    \n";

    Vnm_print(2, usage);
    exit(rc);
}

/**
 * @brief  Invert coordinate transform (A, b) to find (C, d) such that
 *         x = A*y + b
 *         y = C*x + d
 * @param  rotMat  Set to rotation matrix
 * @param  dispVec  Set to displacement vector
 * @return 1 if successful, 0 otherwise */
int invertTransform(double A[3][3], double b[3], double C[3][3], double d[3]) {

    double detA;

    /* Compute the determinant of A */
    detA = - A[0][2]*A[1][1]*A[2][0] + A[0][1]*A[1][2]*A[2][0] \
           + A[0][2]*A[1][0]*A[2][1] - A[0][0]*A[1][2]*A[2][1] \
           - A[0][1]*A[1][0]*A[2][2] + A[0][0]*A[1][1]*A[2][2];
    if (detA < VSMALL) {
        Vnm_print(2, "Error!  Your matrix is singular; det = %g!\n", detA);
        return 0;
    }

    /* Compute C */
    C[0][0] = (-A[1][2]*A[2][1]+A[1][1]*A[2][2])/detA;
    C[0][1] = ( A[0][2]*A[2][1]-A[0][1]*A[2][2])/detA;
    C[0][2] = (-A[0][2]*A[1][1]+A[0][1]*A[1][2])/detA;
    C[1][0] = ( A[1][2]*A[2][0]-A[1][0]*A[2][2])/detA;
    C[1][1] = (-A[0][2]*A[2][0]+A[0][0]*A[2][2])/detA;
    C[1][2] = ( A[0][2]*A[1][0]-A[0][0]*A[1][2])/detA;
    C[2][0] = (-A[1][1]*A[2][0]+A[1][0]*A[2][1])/detA;
    C[2][1] = ( A[0][1]*A[2][0]-A[0][0]*A[2][1])/detA;
    C[2][2] = (-A[0][1]*A[1][0]+A[0][0]*A[1][1])/detA;

    printf("%4.3f %4.3f %4.3f\n",
            A[0][0]*C[0][0] + A[0][1]*C[1][0] + A[0][2]*C[2][0],
            A[0][0]*C[0][1] + A[0][1]*C[1][1] + A[0][2]*C[2][1],
            A[0][0]*C[0][2] + A[0][1]*C[1][2] + A[0][2]*C[2][2]);
    printf("%4.3f %4.3f %4.3f\n",
            A[1][0]*C[0][0] + A[1][1]*C[1][0] + A[1][2]*C[2][0],
            A[1][0]*C[0][1] + A[1][1]*C[1][1] + A[1][2]*C[2][1],
            A[1][0]*C[0][2] + A[1][1]*C[1][2] + A[1][2]*C[2][2]);
    printf("%4.3f %4.3f %4.3f\n",
            A[2][0]*C[0][0] + A[2][1]*C[1][0] + A[2][2]*C[2][0],
            A[2][0]*C[0][1] + A[2][1]*C[1][1] + A[2][2]*C[2][1],
            A[2][0]*C[0][2] + A[2][1]*C[1][2] + A[2][2]*C[2][2]);

    /* Compute d */
    d[0] = -(C[0][0]*b[0]+C[0][1]*b[1]+C[0][2]*b[2]);
    d[1] = -(C[1][0]*b[0]+C[1][1]*b[1]+C[1][2]*b[2]);
    d[2] = -(C[2][0]*b[0]+C[2][1]*b[1]+C[2][2]*b[2]);

    return 1;
}

/**
 * @brief  Read transformation from file
 * @param  path  Path to file
 * @param  rotMat  Set to rotation matrix
 * @param  dispVec  Set to displacement vector
 * @return 1 if successful, 0 otherwise */
int readTransform(char *path, double rotMat[3][3], double dispVec[3]) {

    Vio *sock = VNULL;
    char tok[VMAX_BUFSIZE];
    int rc;

    Vnm_print(1, "Reading coordinate transform from %s...\n", path);
    sock = Vio_ctor("FILE", "ASC", VNULL, path, "r");
    if (sock == VNULL) {
        Vnm_print(2, "Problem opening virtual socket %s!\n", path);
        return 0;
    }
    if (Vio_accept(sock, 0) < 0) {
        Vnm_print(2, "Problem accepting virtual socket %s!\n", path);
        return 0;
    }
    Vio_setWhiteChars(sock, " =,;\t\n");
    Vio_setCommChars(sock, "#%");
    rc = Vio_scanf(sock, "%s", tok);
    if (rc == 1) rc = sscanf(tok, "%lf", &(rotMat[0][0]));
    if (rc != 1) {
        Vnm_print(1, "Error while reading a11!\n");
        return 0;
    }
    rc = Vio_scanf(sock, "%s", tok);
    if (rc == 1) rc = sscanf(tok, "%lf", &(rotMat[0][1]));
    if (rc != 1) {
        Vnm_print(1, "Error while reading a12!\n");
        return 0;
    }
    rc = Vio_scanf(sock, "%s", tok);
    if (rc == 1) rc = sscanf(tok, "%lf", &(rotMat[0][2]));
    if (rc != 1) {
        Vnm_print(1, "Error while reading a13!\n");
        return 0;
    }
    rc = Vio_scanf(sock, "%s", tok);
    if (rc == 1) rc = sscanf(tok, "%lf", &(rotMat[1][0]));
    if (rc != 1) {
        Vnm_print(1, "Error while reading a21!\n");
        return 0;
    }
    rc = Vio_scanf(sock, "%s", tok);
    if (rc == 1) rc = sscanf(tok, "%lf", &(rotMat[1][1]));
    if (rc != 1) {
        Vnm_print(1, "Error while reading a22!\n");
        return 0;
    }
    rc = Vio_scanf(sock, "%s", tok);
    if (rc == 1) rc = sscanf(tok, "%lf", &(rotMat[1][2]));
    if (rc != 1) {
        Vnm_print(1, "Error while reading a23!\n");
        return 0;
    }
    rc = Vio_scanf(sock, "%s", tok);
    if (rc == 1) rc = sscanf(tok, "%lf", &(rotMat[2][0]));
    if (rc != 1) {
        Vnm_print(1, "Error while reading a31!\n");
        return 0;
    }
    rc = Vio_scanf(sock, "%s", tok);
    if (rc == 1) rc = sscanf(tok, "%lf", &(rotMat[2][1]));
    if (rc != 1) {
        Vnm_print(1, "Error while reading a32!\n");
        return 0;
    }
    rc = Vio_scanf(sock, "%s", tok);
    if (rc == 1) rc = sscanf(tok, "%lf", &(rotMat[2][2]));
    if (rc != 1) {
        Vnm_print(1, "Error while reading a33!\n");
        return 0;
    }
    rc = Vio_scanf(sock, "%s", tok);
    if (rc == 1) rc = sscanf(tok, "%lf", &(dispVec[0]));
    if (rc != 1) {
        Vnm_print(1, "Error while reading b1!\n");
        return 0;
    }
    rc = Vio_scanf(sock, "%s", tok);
    if (rc == 1) rc = sscanf(tok, "%lf", &(dispVec[1]));
    if (rc != 1) {
        Vnm_print(1, "Error while reading b2!\n");
        return 0; } rc = Vio_scanf(sock, "%s", tok);
    if (rc == 1) rc = sscanf(tok, "%lf", &(dispVec[2]));
    if (rc != 1) {
        Vnm_print(1, "Error while reading b3!\n");
        return 0;
    }
    Vio_acceptFree(sock);
    Vio_dtor(&sock);

    return 1;
}

/**
 * @brief  Read a grid
 * @param  grid  Pointer to Vgrid file to be read
 * @param  path  Path to read from
 * @param  format  Format to read
 * @return 1 if successful, 0 otherwise */
int readGrid(Vgrid **grid, char *path, Vdata_Format format) {

    *grid = Vgrid_ctor(0, 0, 0,
            0.0, 0.0, 0.0,
            0.0, 0.0, 0.0,
            VNULL);

    switch (format) {
        case VDF_DX:
            return Vgrid_readDX(*grid, "FILE", "ASC", VNULL, path);
            break;
        case VDF_UHBD:
            Vnm_print(2, "Sorry, UHBD input not supported yet!\n");
            return 0;
            break;
        case VDF_AVS:
            Vnm_print(2, "Sorry, AVS input not supported yet!\n");
            return 0;
            break;
        default:
            Vnm_print(2, "Unknown data format (%d)!\n", format);
            return 0;
    }

    return 1;
}

int main(int argc, char **argv) {

    /* *************** VARIABLES ******************* */
    int i, j, k, onGridS1, onGridV1, onGridS2, onGridV2, nx, ny, nz;
    double hx, hy, hzed, xmin, ymin, zmin, dvol, svol, gvol;
    double norm1_L1, norm1_L2, snorm1_H1, norm1_H1;
    double norm2_L1, norm2_L2, snorm2_H1, norm2_H1;
    double normDiff_L1, normDiff_L2, snormDiff_H1, normDiff_H1;
    double ip_L2, ip_H1;
    double val1, val2, sval1, sval2, mval1, mval2, p1[3], p2[3];
    double dval, gval1[3], gval2[3];
    Vgrid *scalar1, *scalar2, *mask1, *mask2;
    double rotMat2to1[3][3], dispVec2to1[3];
    double rotMat1to2[3][3], dispVec1to2[3];
    char scalar1Path[VMAX_ARGLEN];
    int gotScalar1 = 0;
    char scalar2Path[VMAX_ARGLEN];
    int gotScalar2 = 0;
    char transformPath[VMAX_ARGLEN];
    int gotTransform = 0;
    char mask1Path[VMAX_ARGLEN];
    int gotMask1 = 0;
    char mask2Path[VMAX_ARGLEN];
    int gotMask2 = 0;
    Vdata_Format format;
    int gotFormat = 0;
    char *tstr, *targ;

    /* *************** CHECK INVOCATION ******************* */
    Vio_start();
    /* Parse args */
    for (i=1; i<argc; i++) {
        targ = argv[i];
        /* FORMAT */
        tstr = strstr(targ, "format");
        if (tstr != NULL) {
            tstr = tstr + 7;
            if (strcmp(tstr, "dx") == 0) {
                format = VDF_DX;
                gotFormat = 1;
            } else {
                Vnm_print(2, "Error!  Unknown format (%s)!\n", tstr);
                usage(2);
            }
        }
        /* SCALAR 1 */
        tstr = strstr(targ, "scalar1");
        if (tstr != NULL) {
            tstr = tstr + 8;
            strncpy(scalar1Path, tstr, VMAX_ARGLEN);
            gotScalar1 = 1;
        }
        /* SCALAR 2 */
        tstr = strstr(targ, "scalar2");
        if (tstr != NULL) {
            tstr = tstr + 8;
            strncpy(scalar2Path, tstr, VMAX_ARGLEN);
            gotScalar2 = 1;
        }
        /* TRANSFORM */
        tstr = strstr(targ, "transform");
        if (tstr != NULL) {
            tstr = tstr + 10;
            strncpy(transformPath, tstr, VMAX_ARGLEN);
            gotTransform = 1;
        }
        /* HELP */
        tstr = strstr(targ, "help");
        if (tstr != NULL) usage(0);
        /* MASK 1 */
        tstr = strstr(targ, "mask1");
        if (tstr != NULL) {
            tstr = tstr + 6;
            strncpy(mask1Path, tstr, VMAX_ARGLEN);
            gotMask1 = 1;
        }
        /* MASK 2 */
        tstr = strstr(targ, "mask2");
        if (tstr != NULL) {
            tstr = tstr + 6;
            strncpy(mask2Path, tstr, VMAX_ARGLEN);
            gotMask2 = 1;
        }
    }
    /* Check and print args */
    if (!gotFormat) {
        Vnm_print(2, "Error!  --format not specified!\n");
        usage(2);
    } else {
        switch (format) {
            case VDF_DX:
                Vnm_print(1, "format:  OpenDX\n");
                break;
            case VDF_UHBD:
                Vnm_print(1, "format:  UHBD\n");
                break;
            case VDF_AVS:
                Vnm_print(1, "format:  AVS\n");
                break;
            default:
                Vnm_print(2, "Error!  Unknown format (%d)!\n", format);
                usage(2);
        }
    }
    if (!gotScalar1) {
        Vnm_print(2, "Error!  --scalar1 not specified!\n");
        usage(2);
    } else {
        Vnm_print(1, "Data set 1:  %s\n", scalar1Path);
    }
    if (!gotScalar2) {
        Vnm_print(2, "Error!  --scalar2 not specified!\n");
        usage(2);
    } else {
        Vnm_print(1, "Data set 2:  %s\n", scalar2Path);
    }
    if (gotTransform) {
        Vnm_print(1, "Transform:  %s\n", transformPath);
    }
    if (gotMask1) {
        Vnm_print(1, "Mask 1:  %s\n", mask1Path);
    }
    if (gotMask2) {
        Vnm_print(1, "Mask 2:  %s\n", mask2Path);
    }

    /* Parse transform matrix */
    if (!gotTransform) {
        Vnm_print(1, "Setting coordinate transform to identity...\n");
        rotMat2to1[0][0] = 1.0; rotMat2to1[0][1] = 0.0; rotMat2to1[0][2] = 0.0;
        rotMat2to1[1][0] = 0.0; rotMat2to1[1][1] = 1.0; rotMat2to1[1][2] = 0.0;
        rotMat2to1[2][0] = 0.0; rotMat2to1[2][1] = 0.0; rotMat2to1[2][2] = 1.0;
        dispVec2to1[0] = 0.0; dispVec2to1[1] = 0.0; dispVec2to1[2] = 0.0;
    } else {
        if (!readTransform(transformPath, rotMat2to1, dispVec2to1)) {
            Vnm_print(2, "Error reading transformation matrix!\n");
            return 2;
        }
    }
    Vnm_print(1, "Rotation matrix for set 2 into set 1:\n");
    Vnm_print(1, "  %1.12E %1.12E %1.12E\n",
            rotMat2to1[0][0], rotMat2to1[0][1], rotMat2to1[0][2]);
    Vnm_print(1, "  %1.12E %1.12E %1.12E\n",
            rotMat2to1[1][0], rotMat2to1[1][1], rotMat2to1[1][2]);
    Vnm_print(1, "  %1.12E %1.12E %1.12E\n",
            rotMat2to1[2][0], rotMat2to1[2][1], rotMat2to1[2][2]);
    Vnm_print(1, "Displacement vector for set 2 into set 1:\n");
    Vnm_print(1, "  %1.12E %1.12E %1.12E\n",
            dispVec2to1[0], dispVec2to1[1], dispVec2to1[2]);

    /* Invert transformation */
    Vnm_print(1, "Inverting coordinate transform...\n");
    if (!invertTransform(rotMat2to1, dispVec2to1, rotMat1to2, dispVec1to2)) {
        Vnm_print(2, "Error inverting transformation!\n");
        return 2;
    }
    Vnm_print(1, "Rotation matrix for set 1 into set 2:\n");
    Vnm_print(1, "  %1.12E %1.12E %1.12E\n",
            rotMat1to2[0][0], rotMat1to2[0][1], rotMat1to2[0][2]);
    Vnm_print(1, "  %1.12E %1.12E %1.12E\n",
            rotMat1to2[1][0], rotMat1to2[1][1], rotMat1to2[1][2]);
    Vnm_print(1, "  %1.12E %1.12E %1.12E\n",
            rotMat1to2[2][0], rotMat1to2[2][1], rotMat1to2[2][2]);
    Vnm_print(1, "Displacement vector for set 2 into set 1:\n");
    Vnm_print(1, "  %1.12E %1.12E %1.12E\n",
            dispVec1to2[0], dispVec1to2[1], dispVec1to2[2]);


    /* Read scalar set 1 */
    Vnm_print(1, "Reading scalar data set 1 from %s...\n", scalar1Path);
    if (!readGrid(&scalar1, scalar1Path, format)) {
        Vnm_print(2, "Error reading scalar data set 1!\n");
        return 2;
    }
    Vnm_print(1, "Read %d x %d x %d grid.\n",
            scalar1->nx, scalar1->ny, scalar1->nz);

    /* Read scalar set 2 */
    Vnm_print(1, "Reading scalar data set 2 from %s...\n", scalar2Path);
    if (!readGrid(&scalar2, scalar2Path, format)) {
        Vnm_print(2, "Error reading scalar data set 2!\n");
        return 2;
    }
    Vnm_print(1, "Read %d x %d x %d grid.\n",
            scalar2->nx, scalar2->ny, scalar2->nz);

    /* Read mask 1 */
    if (gotMask1) {
        Vnm_print(1, "Reading mask data set 1 from %s...\n", mask1Path);
        if (!readGrid(&mask1, mask1Path, format)) {
            Vnm_print(2, "Error reading mask data set 1!\n");
            return 2;
        }
        Vnm_print(1, "Read %d x %d x %d grid.\n",
                mask1->nx, mask1->ny, mask1->nz);
    }

    /* Read mask 2 */
    if (gotMask2) {
        Vnm_print(1, "Reading mask data set 2 from %s...\n", mask2Path);
        if (!readGrid(&mask2, mask2Path, format)) {
            Vnm_print(2, "Error reading mask data set 2!\n");
            return 2;
        }
        Vnm_print(1, "Read %d x %d x %d grid.\n",
                mask2->nx, mask2->ny, mask2->nz);
    }

    /* Calculate relative L2 norm of difference */
    Vnm_print(1, "Calculating similarity measures...\n");
    nx = scalar1->nx; ny = scalar1->ny; nz = scalar1->nz;
    hx = scalar1->hx; hy = scalar1->hy; hzed = scalar1->hzed;
    dvol = (hx*hy*hzed);
    xmin = scalar1->xmin; ymin = scalar1->ymin; zmin = scalar1->zmin;
    norm1_L1 = 0; norm1_L2 = 0; snorm1_H1 = 0; norm1_H1 = 0;
    norm2_L1 = 0; norm2_L2 = 0; snorm2_H1 = 0; norm2_H1 = 0;
    normDiff_L1 = 0; normDiff_L2 = 0; snormDiff_H1 = 0; normDiff_H1 = 0;
    ip_L2 = 0; ip_H1 = 0;
    svol = 0; gvol = 0;
    for (i=0; i<nx; i++) {
        p1[0] = i*hx + xmin;
        for (j=0; j<ny; j++) {
            p1[1] = j*hy + ymin;
            for (k=0; k<nz; k++) {

                /* Grid 1 values */
                p1[2] = k*hzed + zmin;
                onGridS1 = Vgrid_value(scalar1, p1, &sval1);
                onGridV1 = Vgrid_gradient(scalar1, p1, gval1);
                if (gotMask1) onGridS1 = Vgrid_value(mask1, p1, &mval1);
                else mval1 = 1.0;

                /* Grid 2 values */
                p2[0] = rotMat1to2[0][0]*p1[0] + rotMat1to2[0][1]*p1[1] \
                    + rotMat1to2[0][2]*p1[2] + dispVec1to2[0];
                p2[1] = rotMat1to2[1][0]*p1[0] + rotMat1to2[1][1]*p1[1] \
                    + rotMat1to2[1][2]*p1[2] + dispVec1to2[1];
                p2[2] = rotMat1to2[2][0]*p1[0] + rotMat1to2[2][1]*p1[1] \
                    + rotMat1to2[2][2]*p1[2] + dispVec1to2[2];
                onGridS2 = Vgrid_value(scalar2, p2, &sval2);
                onGridV2 = Vgrid_gradient(scalar2, p2, gval2);
                if (gotMask2) onGridS2 = Vgrid_value(mask2, p2, &mval2);
                else mval2 = 1.0;

                /* Measures based on scalars */
                if (onGridS1 && onGridS2) {
                    val1 = sval1*mval1;
                    val2 = sval2*mval2;
                    dval = mval1*mval2*(sval1 - sval2);

                    /* L2 */
                    norm1_L2 += VSQR(val1);
                    norm2_L2 += VSQR(val2);
                    normDiff_L2 += VSQR(dval);
                    ip_L2 += (val2*val1);
                    /* L1 */
                    norm1_L1 += VABS(val1);
                    norm2_L1 += VABS(val2);
                    normDiff_L1 += VABS(dval);
                    /* Volume */
                    svol += dvol;

                    if (isnan(norm1_L2) || isnan(norm2_L2)) {
                        Vnm_print(2, "ERROR!  Got NaN!\n");
                        Vnm_print(2, "p1 = (%1.12E, %1.12E, %1.12E)\n",
                                p1[0], p1[1], p1[2]);
                        Vnm_print(2, "p2 = (%1.12E, %1.12E, %1.12E)\n",
                                p2[0], p2[1], p2[2]);
                        Vnm_print(2, "mval1 = %1.12E\n", mval1);
                        Vnm_print(2, "mval2 = %1.12E\n", mval2);
                        Vnm_print(2, "sval1 = %1.12E\n", sval1);
                        Vnm_print(2, "sval2 = %1.12E\n", sval2);
                        Vnm_print(2, "val1 = %1.12E\n", val1);
                        Vnm_print(2, "val2 = %1.12E\n", val2);
                        Vnm_print(2, "dval = %1.12E\n", dval);
                        VASSERT(0);
                    }
                }

                /* Measures based on gradients */
                if (onGridV1 && onGridV2 && onGridS1 && onGridS2) {
                    val1 = mval1*(VSQR(gval1[0]) + VSQR(gval1[1]) \
                            + VSQR(gval1[2]));
                    val2 = mval2*(VSQR(gval2[0]) + VSQR(gval2[1]) \
                            + VSQR(gval2[2]));
                    dval = mval1*mval2*(VSQR(gval1[0]-gval2[0]) \
                            + VSQR(gval1[1]-gval2[1]) \
                            + VSQR(gval1[2]-gval2[2]));
                    snorm1_H1 += VSQR(val1);
                    snorm2_H1 += VSQR(val2);
                    snormDiff_H1 += VSQR(dval);
                    ip_H1 += (val1*val2);
                    gvol += dvol;
                }
            }
        }
    }
    /* Volumes */
    Vnm_print(1, "Volume used to calculate L2 and L1 measures = %1.12E\n",
            svol);
    Vnm_print(1, "Volume used to calculate H1 measures        = %1.12E\n",
            gvol);
    /* L2 */
    printf("norm1_L2^2 = %1.12E\n", norm1_L2);
    norm1_L2 = VSQRT(norm1_L2*dvol);
    printf("norm1_L2 = %1.12E\n", norm1_L2);
    printf("norm2_L2^2 = %1.12E\n", norm2_L2);
    norm2_L2 = VSQRT(norm2_L2*dvol);
    printf("norm2_L2 = %1.12E\n", norm2_L2);
    printf("normDiff_L2^2 = %1.12E\n", normDiff_L2);
    normDiff_L2 = VSQRT(normDiff_L2*dvol);
    printf("normDiff_L2 = %1.12E\n", normDiff_L2);
    ip_L2 = ip_L2*dvol;
    Vnm_print(1, "Set 1 absolute L2 norm           = %1.12E\n", norm1_L2);
    Vnm_print(1, "Set 2 absolute L2 norm           = %1.12E\n", norm2_L2);
    Vnm_print(1, "Difference absolute L2 norm      = %1.12E\n", normDiff_L2);
    Vnm_print(1, "Difference relative L2 norm      = %1.12E\n",
            VSQR(normDiff_L2)/(norm1_L2*norm2_L2));
    Vnm_print(1, "Absolute L2 inner product        = %1.12E\n", ip_L2);
    Vnm_print(1, "Hodgkin L2 inner product         = %1.12E\n",
            2*ip_L2/(VSQR(norm1_L2)+VSQR(norm2_L2)));
    Vnm_print(1, "Carbo L2 inner product           = %1.12E\n",
            ip_L2/(norm1_L2*norm2_L2));
    /* L1 */
    norm1_L1 = (norm1_L2*dvol);
    norm2_L1 = (norm2_L2*dvol);
    normDiff_L1 = (normDiff_L2*dvol);
    Vnm_print(1, "Set 1 absolute L1 norm           = %1.12E\n", norm1_L1);
    Vnm_print(1, "Set 2 absolute L1 norm           = %1.12E\n", norm2_L1);
    Vnm_print(1, "Difference absolute L1 norm      = %1.12E\n", normDiff_L1);
    Vnm_print(1, "Difference relative L1 norm      = %1.12E\n",
            VSQR(normDiff_L1)/(norm1_L1*norm2_L1));
    /* H1 */
    snorm1_H1 = VSQRT(snorm1_H1*dvol);
    snorm2_H1 = VSQRT(snorm2_H1*dvol);
    snormDiff_H1 = VSQRT(snormDiff_H1*dvol);
    norm1_H1 = VSQRT(VSQR(snorm1_H1)+VSQR(norm1_L2));
    norm2_H1 = VSQRT(VSQR(snorm2_H1)+VSQR(norm2_L2));
    normDiff_H1 = VSQRT(VSQR(snormDiff_H1)+VSQR(normDiff_L2));
    ip_H1 = ip_H1*dvol;
    Vnm_print(1, "Set 1 absolute H1 norm           = %1.12E\n", norm1_H1);
    Vnm_print(1, "Set 2 absolute H1 norm           = %1.12E\n", norm2_H1);
    Vnm_print(1, "Difference absolute H1 norm      = %1.12E\n", normDiff_H1);
    Vnm_print(1, "Difference relative H1 norm      = %1.12E\n",
            VSQR(normDiff_H1)/(norm1_L2*norm2_H1));
    Vnm_print(1, "Set 1 absolute H1 semi-norm      = %1.12E\n", snorm1_H1);
    Vnm_print(1, "Set 2 absolute H1 semi-norm      = %1.12E\n", snorm2_H1);
    Vnm_print(1, "Difference absolute H1 semi-norm = %1.12E\n", snormDiff_H1);
    Vnm_print(1, "Absolute H1 inner product        = %1.12E\n", ip_H1);
    Vnm_print(1, "Hodgkin H1 inner product         = %1.12E\n",
            2*ip_H1/(VSQR(snorm1_H1)+VSQR(snorm2_H1)));
    Vnm_print(1, "Carbo H1 inner product           = %1.12E\n",
            ip_H1/(snorm1_H1*snorm2_H1));
    Vnm_print(1, "Absolute H1+L2 inner product     = %1.12E\n",
            (ip_H1+ip_L2));
    Vnm_print(1, "Hodgkin H1+L2 inner product      = %1.12E\n",
            2*(ip_H1+ip_L2)/(VSQR(norm1_H1)+VSQR(norm2_H1)));
    Vnm_print(1, "Carbo H1+L2 inner product        = %1.12E\n",
            (ip_H1+ip_L2)/(norm1_H1*norm2_H1));

    return 0;
}
