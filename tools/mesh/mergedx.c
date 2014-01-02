/**
 *  @file    mergedx.c
 *  @author  Stephen Bond and Nathan Baker
 *  @brief   Program that merges OpenDX files
 *  @version $Id$
 */

#include "apbs.h"

#define SHORTINT short
#define IJK(i,j,k)  (((k)*(nx)*(ny))+((j)*(nx))+(i))
#define INTERVAL(x,a,b) (((x) >= (a)) && ((x) <= (b)))

VEMBED(rcsid="$Id$")

VPRIVATE int Vgrid_readDXhead(Vgrid *thee,
  const char *iodev, const char *iofmt, const char *thost, const char *fname);
VPRIVATE int Vgrid_value2(Vgrid *thee, double pt[3], double *value);
VPRIVATE int Char_parseARGV(int argc, char **argv, int *nx, int *ny, int *nz,
  int *pad, char ***fnams, int *numfnams, char *outname, int *vflag);

VPRIVATE char *MCwhiteChars = " =,;\t\n";
VPRIVATE char *MCcommChars  = "#%";

int main(int argc, char **argv) {

    /* *************** VARIABLES ******************* */
    int i, j, k, vlev = 1, vvlev = 0, vflag = 1;
    int nx, ny, nz, count, numfnams;
    double pt[3],value;
    double xmin, ymin, zmin, xmax, ymax, zmax;
    char **fnams = VNULL;
    SHORTINT *carray = VNULL;
    char *usage0 = "[FLAGS] nx ny nz file1.dx [file2.dx ...]\n";
    char *req0  = "nx ny nz        Grid points on the merged grid";
    char *req1  = "file1.dx        Names of unmerged grid files";
    char *flag0 = "-v               Verbose                  (default: off)";
    char *flag1 = "-quiet           Silent                   (default: off)";
    char *flag2 = "-pad integer     Num. of pad grid points  (default: 1  )";
    char *flag3 = "-o filename.dx   Output file    (default: gridmerged.dx)";
    char *note0 = "Each subgrid is extended by the number of pad points,";
    char *note1 = "which is often necessary to fill gaps between the grids.";
    char *note2 = "Any overlap between subgrids is resolved by averaging.";
    char *snam = "# main:  ";
    char outname[80];
    Vgrid *grid, *mgrid;
    int pad = 1;

    Vio_start();
    sprintf(outname,"gridmerged.dx");

    /* **************** OBSOLETE WARNING ***************** */
    printf("WARNING: mergedx is deprecated. Please consider using mergedx2\n");

    /* **************** PARSE INPUT ARGS ***************** */

    if ( Char_parseARGV(argc, argv, &nx, &ny, &nz, &pad,
                        &fnams, &numfnams, outname, &vflag) != 0 ) {
        Vnm_print(2,"\nImproper or Unrecognized Switches?\nUsage: ");
        Vnm_print(2,"%s %s\n",argv[0],usage0);
        Vnm_print(2,"Input:\t\t%s\n\t\t%s\n\n", req0, req1);
        Vnm_print(2,"Flags:\t\t%s\n\t\t%s\n\t\t%s\n\t\t%s\n\n",
                  flag0, flag1, flag2, flag3);
        Vnm_print(2,"Notes:\t\t%s\n\t\t%s\n\t\t%s\n",
                  note0, note1, note2);
        return -1;
    }

    if (vflag == 1) {
    vlev = 1;
    vvlev = 0;
    } else if (vflag) {
        vlev = 2;
        vvlev = 2;
    } else {
        vlev = 0;
        vvlev = 0;
    }

    /* *********** PREPARE MERGED GRID OBJECT ************* */
    mgrid = Vgrid_ctor(nx, ny, nz, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, VNULL);
    mgrid->xmin = VLARGE; mgrid->xmax = -VLARGE;
    mgrid->ymin = VLARGE; mgrid->ymax = -VLARGE;
    mgrid->zmin = VLARGE; mgrid->zmax = -VLARGE;

    /* *************** GET FILE HEADERS ******************* */
    Vnm_print(vlev, "%s Reading Headers...\n",snam);
    grid = Vgrid_ctor(0, 0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, VNULL);
    for(count=0; count<numfnams; count++) {
        Vnm_print(vvlev, "%s  Reading header from %s...\n",snam,
                  fnams[count]);
        Vnm_tstart(26, "HEADER READ");
        Vgrid_readDXhead(grid, "FILE", "ASC", VNULL, fnams[count]);
        Vnm_tstop(26, "HEADER READ");
        /* set the merged grid bounds to include all the subgrids */
        if( grid->xmin < mgrid->xmin ) mgrid->xmin = grid->xmin;
        if( grid->xmax > mgrid->xmax ) mgrid->xmax = grid->xmax;
        if( grid->ymin < mgrid->ymin ) mgrid->ymin = grid->ymin;
        if( grid->ymax > mgrid->ymax ) mgrid->ymax = grid->ymax;
        if( grid->zmin < mgrid->zmin ) mgrid->zmin = grid->zmin;
        if( grid->zmax > mgrid->zmax ) mgrid->zmax = grid->zmax;
    }

    /* set the grid increment for the merged grid */
    mgrid->hx   = (mgrid->xmax - mgrid->xmin)/(mgrid->nx - 1);
    mgrid->hy   = (mgrid->ymax - mgrid->ymin)/(mgrid->ny - 1);
    mgrid->hzed = (mgrid->zmax - mgrid->zmin)/(mgrid->nz - 1);

    /* print out the dimensions of the merged grid */
    Vnm_print(vlev, "%s Dimensions of the merged grid\n",snam);
    Vnm_print(vlev, "%s nx = %d, ny = %d, nz = %d\n",snam,
              mgrid->nx, mgrid->ny, mgrid->nz);
    Vnm_print(vlev, "%s hx = %g, hy = %g, hz = %g\n",snam,
              mgrid->hx, mgrid->hy, mgrid->hzed);
    Vnm_print(vlev, "%s xmin = %g, ymin = %g, zmin = %g\n",snam,
              mgrid->xmin, mgrid-> ymin, mgrid->zmin);
    Vnm_print(vlev, "%s xmax = %g, ymax = %g, zmax = %g\n",snam,
              mgrid->xmax, mgrid-> ymax, mgrid->zmax);

    mgrid->data = (double *)
       Vmem_malloc(mgrid->mem,(mgrid->nx*mgrid->ny*mgrid->nz),sizeof(double));
    mgrid->ctordata = 1;
    carray = (SHORTINT *)
       Vmem_malloc(VNULL, (mgrid->nx*mgrid->ny*mgrid->nz), sizeof(SHORTINT) );

    /* initialize the data of the merged grid with zeros */
    nx = mgrid->nx;
    ny = mgrid->ny;
    nz = mgrid->nz;
    for (i=0; i<nx; i++) {
        for (j=0; j<ny; j++) {
            for (k=0; k<nz; k++) {
                (mgrid->data)[IJK(i,j,k)] = 0.0;
                carray[IJK(i,j,k)] = 0;
            }
        }
    }

    /* ************** MERGE THE GRID FILES **************** */
    Vnm_print(vlev, "%s Reading and Merging...\n",snam);
    for (count=0; count<numfnams; count++) {
        Vnm_print(vvlev, "%s  Reading data from %s...\n",snam,fnams[count]);
        Vnm_tstart(26, "DATA READ");
        Vgrid_readDX(grid, "FILE", "ASC", VNULL, fnams[count]);
        Vnm_tstop(26, "DATA READ");
        Vnm_print(vvlev, "%s  Merging data from %s...\n",snam,fnams[count]);
        Vnm_tstart(26, "MERGING");
        xmin = grid->xmin - pad*grid->hx   - VSMALL;
        ymin = grid->ymin - pad*grid->hy   - VSMALL;
        zmin = grid->zmin - pad*grid->hzed - VSMALL;
        xmax = grid->xmax + pad*grid->hx   + VSMALL;
        ymax = grid->ymax + pad*grid->hy   + VSMALL;
        zmax = grid->zmax + pad*grid->hzed + VSMALL;
        Vnm_print(vvlev, "%s  MIN (%g,%g,%g) IMIN (%g,%g,%g)\n",snam,
                          grid->xmin,grid->ymin,grid->zmin,xmin,ymin,zmin);
        Vnm_print(vvlev, "%s  MAX (%g,%g,%g) IMAX (%g,%g,%g)\n",snam,
                          grid->xmax,grid->ymax,grid->zmax,xmax,ymax,zmax);
        for (i=0; i<nx; i++) {
            pt[0] = mgrid->xmin + i*mgrid->hx;
            if(INTERVAL(pt[0],xmin,xmax)) {
                for (j=0; j<ny; j++) {
                    pt[1] = mgrid->ymin + j*mgrid->hy;
                    if(INTERVAL(pt[1],ymin,ymax)) {
                        for (k=0; k<nz; k++) {
                            pt[2] = mgrid->zmin + k*mgrid->hzed;
                            if(INTERVAL(pt[2],zmin,zmax)) {
                                if (Vgrid_value2(grid, pt, &value)) {
                                    (mgrid->data)[IJK(i,j,k)] += value;
                                    carray[IJK(i,j,k)] += 1;
                                }
                            }
                        }
                    }
                }
            }
        }
        Vnm_tstop(26, "MERGING");
        Vmem_free(grid->mem,(grid->nx*grid->ny*grid->nz), sizeof(double),
                  (void **)&(grid->data));
        grid->readdata = 0;
        grid->ctordata = 0;
    }
    Vgrid_dtor( &grid );

    mgrid->readdata = 1;

    /* Check for skipped points, and account for overlap */
    nx = mgrid->nx;
    ny = mgrid->ny;
    nz = mgrid->nz;
    for (i=0; i<nx; i++) {
        for (j=0; j<ny; j++) {
            for (k=0; k<nz; k++) {
                if ( carray[IJK(i,j,k)] >= 1 ) {
                    (mgrid->data)[IJK(i,j,k)] /= carray[IJK(i,j,k)];
                } else {
                    Vnm_print(2,"%s %s %s (%g,%g,%g)\n",snam,
                              "Warning: ",
                              "Gap in subgrids at point",
                              mgrid->xmin + i*mgrid->hx,
                              mgrid->ymin + j*mgrid->hy,
                              mgrid->zmin + k*mgrid->hzed );
                }
            }
        }
    }

    /* ************** WRITE THE MERGED GRID **************** */
    Vnm_print(vlev, "%s Writing...\n",snam);
    Vnm_print(vvlev, "%s  Writing merged data to %s...\n",snam,outname);
    Vgrid_writeDX(mgrid, "FILE", "ASC", VNULL, outname,"mergedx",VNULL);

    Vmem_free(VNULL,(mgrid->nx*mgrid->ny*mgrid->nz), sizeof(SHORTINT),
              (void **)&(carray));
    Vgrid_dtor( &mgrid );

    if ( vflag > 1 ) {
        Vnm_print(2,"%s Memory Profiling Information\n",snam);
        Vnm_print(2,"# --------------------------------------"
                  "--------------------------------------\n");
        Vnm_print(2,"#  Footprint        Areas       Malloc         Free"
                  "    Highwater   Class\n");
        Vnm_print(2,"# --------------------------------------"
                  "--------------------------------------\n");
        Vmem_print(VNULL);
        Vmem_printTotal();
        Vnm_print(2,"# --------------------------------------"
                  "--------------------------------------\n");
    }

    return 0;
}

/* ///////////////////////////////////////////////////////////////////////////
// Routine:  Vgrid_readDXhead
//
// Author:   Nathan Baker and Stephen Bond
/////////////////////////////////////////////////////////////////////////// */
VPRIVATE int Vgrid_readDXhead(Vgrid *thee,
  const char *iodev, const char *iofmt, const char *thost, const char *fname) {

    int itmp;
    double dtmp;
    char tok[VMAX_BUFSIZE];
    char *snam = "Vgrid_readDXhead:";
    Vio *sock;

    /* Check to see if the existing data is null and, if not, clear it out */
    if (thee->data != VNULL) {
        Vnm_print(1, "%s  destroying existing data!\n",snam);
        Vmem_free(thee->mem, (thee->nx*thee->ny*thee->nz), sizeof(double),
          (void **)&(thee->data)); }
    thee->readdata = 0;
    thee->ctordata = 0;

    /* Set up the virtual socket */
    sock = Vio_ctor(iodev,iofmt,thost,fname,"r");
    if (sock == VNULL) {
        Vnm_print(2, "%s Problem opening virtual socket %s\n",snam,fname);
        return 0;
    }
    if (Vio_accept(sock, 0) < 0) {
        Vnm_print(2, "%s Problem accepting virtual socket %s\n",snam,fname);
        return 0;
    }

    Vio_setWhiteChars(sock, MCwhiteChars);
    Vio_setCommChars(sock, MCcommChars);

    /* Read in the DX regular positions */
    /* Get "object" */
    VJMPERR2(1 == Vio_scanf(sock, "%s", tok));
    VJMPERR1(!strcmp(tok, "object"));
    /* Get "1" */
    VJMPERR2(1 == Vio_scanf(sock, "%d", &itmp));
    /* Get "class" */
    VJMPERR2(1 == Vio_scanf(sock, "%s", tok));
    VJMPERR1(!strcmp(tok, "class"));
    /* Get "gridpositions" */
    VJMPERR2(1 == Vio_scanf(sock, "%s", tok));
    VJMPERR1(!strcmp(tok, "gridpositions"));
    /* Get "counts" */
    VJMPERR2(1 == Vio_scanf(sock, "%s", tok));
    VJMPERR1(!strcmp(tok, "counts"));
    /* Get nx */
    VJMPERR2(1 == Vio_scanf(sock, "%s", tok));
    VJMPERR1(1 == sscanf(tok, "%d", &(thee->nx)));
    /* Get ny */
    VJMPERR2(1 == Vio_scanf(sock, "%s", tok));
    VJMPERR1(1 == sscanf(tok, "%d", &(thee->ny)));
    /* Get nz */
    VJMPERR2(1 == Vio_scanf(sock, "%s", tok));
    VJMPERR1(1 == sscanf(tok, "%d", &(thee->nz)));
    Vnm_print(0, "%s  Grid dimensions %d x %d x %d grid\n",snam,
              thee->nx, thee->ny, thee->nz);
    /* Get "origin" */
    VJMPERR2(1 == Vio_scanf(sock, "%s", tok));
    VJMPERR1(!strcmp(tok, "origin"));
    /* Get xmin */
    VJMPERR2(1 == Vio_scanf(sock, "%s", tok));
    VJMPERR1(1 == sscanf(tok, "%lf", &(thee->xmin)));
    /* Get ymin */
    VJMPERR2(1 == Vio_scanf(sock, "%s", tok));
    VJMPERR1(1 == sscanf(tok, "%lf", &(thee->ymin)));
    /* Get zmin */
    VJMPERR2(1 == Vio_scanf(sock, "%s", tok));
    VJMPERR1(1 == sscanf(tok, "%lf", &(thee->zmin)));
    Vnm_print(0, "%s  Grid origin = (%g, %g, %g)\n",snam,
              thee->xmin, thee->ymin, thee->zmin);
    /* Get "delta" */
    VJMPERR2(1 == Vio_scanf(sock, "%s", tok));
    VJMPERR1(!strcmp(tok, "delta"));
    /* Get hx */
    VJMPERR2(1 == Vio_scanf(sock, "%s", tok));
    VJMPERR1(1 == sscanf(tok, "%lf", &(thee->hx)));
    /* Get 0.0 */
    VJMPERR2(1 == Vio_scanf(sock, "%s", tok));
    VJMPERR1(1 == sscanf(tok, "%lf", &dtmp));
    VJMPERR1(dtmp == 0.0);
    /* Get 0.0 */
    VJMPERR2(1 == Vio_scanf(sock, "%s", tok));
    VJMPERR1(1 == sscanf(tok, "%lf", &dtmp));
    VJMPERR1(dtmp == 0.0);
    /* Get "delta" */
    VJMPERR2(1 == Vio_scanf(sock, "%s", tok));
    VJMPERR1(!strcmp(tok, "delta"));
    /* Get 0.0 */
    VJMPERR2(1 == Vio_scanf(sock, "%s", tok));
    VJMPERR1(1 == sscanf(tok, "%lf", &dtmp));
    VJMPERR1(dtmp == 0.0);
    /* Get hy */
    VJMPERR2(1 == Vio_scanf(sock, "%s", tok));
    VJMPERR1(1 == sscanf(tok, "%lf", &(thee->hy)));
    /* Get 0.0 */
    VJMPERR2(1 == Vio_scanf(sock, "%s", tok));
    VJMPERR1(1 == sscanf(tok, "%lf", &dtmp));
    VJMPERR1(dtmp == 0.0);
    /* Get "delta" */
    VJMPERR2(1 == Vio_scanf(sock, "%s", tok));
    VJMPERR1(!strcmp(tok, "delta"));
    /* Get 0.0 */
    VJMPERR2(1 == Vio_scanf(sock, "%s", tok));
    VJMPERR1(1 == sscanf(tok, "%lf", &dtmp));
    VJMPERR1(dtmp == 0.0);
    /* Get 0.0 */
    VJMPERR2(1 == Vio_scanf(sock, "%s", tok));
    VJMPERR1(1 == sscanf(tok, "%lf", &dtmp));
    VJMPERR1(dtmp == 0.0);
    /* Get hz */
    VJMPERR2(1 == Vio_scanf(sock, "%s", tok));
    VJMPERR1(1 == sscanf(tok, "%lf", &(thee->hzed)));
    Vnm_print(0, "%s  Grid spacings = (%g, %g, %g)\n",snam,
              thee->hx, thee->hy, thee->hzed);
    /* calculate grid maxima */
    thee->xmax = thee->xmin + (thee->nx-1)*thee->hx;
    thee->ymax = thee->ymin + (thee->ny-1)*thee->hy;
    thee->zmax = thee->zmin + (thee->nz-1)*thee->hzed;

    /* Close off the socket */
    Vio_acceptFree(sock);
    Vio_dtor(&sock);

    return 1;

  VERROR1:
    Vio_dtor(&sock);
    Vnm_print(2, "%s  Format problem with input file <%s>\n",snam,fname);
    return 0;

  VERROR2:
    Vio_dtor(&sock);
    Vnm_print(2, "%s  I/O problem with input file <%s>\n",snam,fname);
    return 0;
}

/* ///////////////////////////////////////////////////////////////////////////
// Routine:  Vgrid_value2
//
// Author:   Nathan Baker and Stephen Bond
/////////////////////////////////////////////////////////////////////////// */
VPUBLIC int Vgrid_value2(Vgrid *thee, double pt[3], double *value) {

    int nx, ny, nz, ihi, jhi, khi, ilo, jlo, klo;
    double hx, hy, hzed, xmin, ymin, zmin, ifloat, jfloat, kfloat;
    double u, dx, dy, dz;

    VASSERT(thee != VNULL);
    VASSERT(thee->ctordata || thee->readdata);

    nx = thee->nx;
    ny = thee->ny;
    nz = thee->nz;
    hx = thee->hx;
    hy = thee->hy;
    hzed = thee->hzed;
    xmin = thee->xmin;
    ymin = thee->ymin;
    zmin = thee->zmin;

    u = 0;

    ifloat = (pt[0] - xmin)/hx;
    jfloat = (pt[1] - ymin)/hy;
    kfloat = (pt[2] - zmin)/hzed;
    ihi = (int)ceil(ifloat);
    jhi = (int)ceil(jfloat);
    khi = (int)ceil(kfloat);
    ilo = (int)floor(ifloat);
    jlo = (int)floor(jfloat);
    klo = (int)floor(kfloat);

    /* If the point is outside the mesh, push it to the mesh */
    if ( ilo < 0 ) {
        ilo = 0;
        ihi = ilo + 1;
        ifloat = (double)(ilo);
    } else if ( ihi >= nx ) {
        ihi = nx - 1;
        ilo = ihi - 1;
        ifloat = (double)(ihi);
    }
    if ( jlo < 0 ) {
        jlo = 0;
        jhi = jlo + 1;
        jfloat = (double)(jlo);
    } else if ( jhi >= ny ) {
        jhi = ny - 1;
        jlo = jhi - 1;
        jfloat = (double)(jhi);
    }
    if ( klo < 0 ) {
        klo = 0;
        khi = klo + 1;
        kfloat = (double)(klo);
    } else if ( khi >= nz ) {
        khi = nz - 1;
        klo = khi - 1;
        kfloat = (double)(khi);
    }

    /* See if we're on the mesh */
    if ((ihi<nx) && (jhi<ny) && (khi<nz) &&
        (ilo>=0) && (jlo>=0) && (klo>=0)) {
        dx = ifloat - (double)(ilo);
        dy = jfloat - (double)(jlo);
        dz = kfloat - (double)(klo);
        u = dx      *dy      *dz      *(thee->data[IJK(ihi,jhi,khi)])
          + dx      *(1.0-dy)*dz      *(thee->data[IJK(ihi,jlo,khi)])
          + dx      *dy      *(1.0-dz)*(thee->data[IJK(ihi,jhi,klo)])
          + dx      *(1.0-dy)*(1.0-dz)*(thee->data[IJK(ihi,jlo,klo)])
          + (1.0-dx)*dy      *dz      *(thee->data[IJK(ilo,jhi,khi)])
          + (1.0-dx)*(1.0-dy)*dz      *(thee->data[IJK(ilo,jlo,khi)])
          + (1.0-dx)*dy      *(1.0-dz)*(thee->data[IJK(ilo,jhi,klo)])
          + (1.0-dx)*(1.0-dy)*(1.0-dz)*(thee->data[IJK(ilo,jlo,klo)]);

        *value = u;
        return 1;

    }

    *value = 0.0;
    return 0;
}

VPRIVATE int Char_parseARGV(int argc, char **argv,
  int *nx, int *ny, int *nz, int *pad, char ***fnams, int *numfnams,
  char *outname, int *vflag)
{
    int i, j, hflag, nflags, sflag;

    i = 1;
    hflag = 0;
    nflags = 0;
    while( i < argc ) {
        if( argv[i][0] == '-' ) {
            nflags++;
            if (!strcmp(argv[i],"-v")) {
                (*vflag) = 2;
            } else if (!strcmp(argv[i],"-quiet")) {
                (*vflag) = 0;
            } else if (!strcmp(argv[i],"-o")) {
                i++;
                if( i < argc ) {
                    nflags++;
                    sprintf(outname,"%s",argv[i]);
                }
            } else if (!strcmp(argv[i],"-pad")) {
                i++;
                if( i < argc ) {
                    nflags++;
                    (*pad) = atoi(argv[i]);
                }
            } else {
                hflag = 1;
            }
        }
        i++;
    }

    /* *************** CHECK INVOCATION ******************* */
    if ((argc - nflags) < 5 || hflag) {
        return 1;
    }

    /* ************* PARSE REMAINING ARGS ****************** */
    i = 1;
    j = 0;
    hflag = 0;
    sflag = 1;
    while(i<argc && sflag) {
        if( argv[i][0] == '-' ) {
            j++;
            if (!strcmp(argv[i],"-o")) {
                i++;
                j++;
            } else if (!strcmp(argv[i],"-pad")) {
                i++;
                j++;
            }
        } else {
            if( (i+2) < argc && nflags == j) {
                (*nx) = atoi(argv[i]);
                (*ny) = atoi(argv[i+1]);
                (*nz) = atoi(argv[i+2]);
                i += 2;
            } else {
                hflag = 1;
            }
            sflag = 0;
        }
        i++;
    }

    if (hflag) {
        return 1;
    }

    (*fnams) = &(argv[i]);
    (*numfnams) = argc - i;

    return 0;
}

