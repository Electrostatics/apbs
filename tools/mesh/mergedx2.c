/**
 *  @file     mergedx2.c
 *  @author  David Gohara, Stephen Bond and Nathan Baker
 *  @brief   Program that merges OpenDX files
 *  @version $Id$
 */

#include "apbs.h"

#define IJK(i,j,k)  (((k)*(nx)*(ny))+((j)*(nx))+(i))
#define INTERVAL(x,a,b) (((x) >= (a)) && ((x) <= (b)))
#define MAX_INPUT_2 512
#define MAX_INPUT_PATH 1024

VEMBED(rcsid="$Id$")

VPRIVATE int Vgrid_readDXhead(Vgrid *thee,
  const char *iodev, const char *iofmt, const char *thost, const char *fname);
VPRIVATE int Vgrid_value2(Vgrid *thee, double pt[3], double *value);
VPRIVATE int Char_parseARGV(int argc, char **argv,
  double *res1, double *res2, double *res3, 
  double *xmin, double *ymin, double *zmin,
  double *xmax, double *ymax, double *zmax,
  int *spec, char *outname, char fnams[MAX_INPUT_2][MAX_INPUT_PATH], int *numfnams,
  Vdata_Format *formatin, Vdata_Format *formatout);

VPRIVATE char *MCwhiteChars = " =,;\t\n";
VPRIVATE char *MCcommChars  = "#%";

void usage(){

	Vnm_print(1,"mergedx2 [FLAGS] file1.dx [file2.dx ...]\n"
				"\n"
				"ARGUMENTS:\n"
				"	file1.dx [file2.dx ...]\n"
				"			The OpenDX files to be merged\n"
				"FLAGS:\n"
				"	-o		Output OpenDX file		(default: gridmerged.dx)\n"
				"	-i		Type of OpenDX input files as: dx for standard, dxbin for binary\n"
				"							(default: dx)\n"
				"	-t		Type of OpenDX output file as: dx for standard, dxbin for binary\n"
				"							(default: dx)\n"
				"	-r		Resolution of gridpoints as: resx yres zres\n"
				"							(default: <1.0, 1.0, 1.0> Angstroms)\n"
				"	-b		Bounds of output map as: xmin ymin zmin xmax ymax zmax\n"
				"							(default: calculates full map)\n"
				"	-s		Print bounds of merged input dx files. Doesn't generate a merged map.\n"
				"							(-s is exclusive of the other FLAGS)\n"
				"	-h		Print this message\n"
				"\n"
				"All FLAGS are optional. Flags must be set prior to listing input files. You must provide at least one\n"
				"OpenDX file. Subsequent files can be listed as a series of names on the command line.\n"
				"\n"
				"Specifying -s with all of the input files listed will run a calculation that will print the current minimum\n"
				"and maximum bounds for all user supplied input files. No output (merged) OpenDX file is produced. The -s\n"
				"flag will cause all other options to be ignored.\n"
				"\n"
				"Specifying -r will allow the user to supply a spacing of grid points in the output OpenDX map. If the\n"
				"specified resolution is smaller than the actual resolution in the input files, upsampling will occur and a\n"
				"message printed to stdout will be passed. The default value is 1.0.\n"
				"\n"
				"The -b flag allows the user to specify a subvolume of the volume occupied by all input OpenDX files. Ranges\n"
				"provided that fall outside the available bounds will cause the program to terminate. To determine the bounds\n"
				"of all input files use the -s option. The order for specifying bounds is:\n"
				"\n"
				"	-b xmin ymin zmin xmax ymax zmax\n"
				"\n"
				"The default values are the full bounds of all input files.\n"
				"\n"
				"Specifying -o will assign an output name to the merged OpenDX file. The default file name is gridmerged.dx.\n"
				"\n"
				"Specifying -i will specify the type of the OpenDX files to be read in, either dx for standard OpenDX format\n"
				"files or dxbin for binary OpenDX format files. The default type is dx, or standard OpenDX.\n"
				"\n"
				"Specifying -t will specify the type of the OpenDX file to be output, either dx for a standard OpenDX format\n"
				"file or dxbin for a binary OpenDX format files. The default type is dx, or standard OpenDX.\n"
				"\n"
				"Examples:\n"
				"\n"
				"	./mergedx2 -r 0.5 0.5 0.5 file1.dx file2.dx\n"
				"\n"
				"	./mergedx2 -b -3.13 -2.0 -2.14 31.0 25.4 22.1 file1.dx file2.dx file3.dx\n"
				"\n"
				"	./mergedx2 -o myfile.dx -r 0.5 0.5 0.5 -b -3.13 -2.0 -2.14 31.0 25.4 22.1 file1.dx file2.dx\n"
				"\n"
				"	./mergedx2 -s\n"
				"\n"
		   );

}

int main(int argc, char **argv) {

	/* *************** VARIABLES ******************* */
	size_t i, j, k, mem_size;
	int spec,warn;
	int nx, ny, nz, count, numfnams;

	double pt[3],value, res1, res2, res3, resx, resy, resz;

	double xmin, ymin, zmin;
	double xmax, ymax, zmax;

	double xminb, yminb, zminb;
	double xmaxb, ymaxb, zmaxb;

	char fnams[MAX_INPUT_2][MAX_INPUT_PATH];
	short *carray = VNULL;

	char *snam = "# main:  ";
	char outname[MAX_INPUT_PATH];

        Vdata_Format formatin;
        Vdata_Format formatout;

	Vgrid *grid, *mgrid;

	/* Set the default values */
	spec = 0;
	warn = 0;
	res1 = 1.0;
	res2 = 1.0;
	res3 = 1.0;
	xmin = ymin = zmin = 0.0;
	xmax = ymax = zmax = 0.0;
	sprintf(outname,"gridmerged.dx");
        formatin = VDF_DX;
        formatout = VDF_DX;

	/* Begin processing command line options */

	/* Check the invocation */
	if(argc <= 1){ usage(); return 1; }

	if(Char_parseARGV(argc, argv, &res1, &res2, &res3, &xmin, &ymin, &zmin,
					  &xmax, &ymax, &zmax, &spec, outname, fnams,
                                          &numfnams, &formatin, &formatout))
	{
		usage();
		return 1;
	}

	/* Start the I/O processing */
	Vio_start();

	/* We now allow different resolutions for each of the three axes. */
	resx = res1;
	resy = res2;
	resz = res3;

	/* *********** PREPARE MERGED GRID OBJECT ************* */
	mgrid = Vgrid_ctor(0, 0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, VNULL);
	mgrid->xmin = VLARGE; mgrid->xmax = -VLARGE;
	mgrid->ymin = VLARGE; mgrid->ymax = -VLARGE;
	mgrid->zmin = VLARGE; mgrid->zmax = -VLARGE;

	/* *************** GET FILE HEADERS ******************* */
	Vnm_print(1, "%s Reading Headers...\n",snam);
	grid = Vgrid_ctor(0, 0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, VNULL);
	for(count=0; count<numfnams; count++) {
		Vnm_print(0, "%s  Reading header from %s...\n",snam, fnams[count]);
		Vgrid_readDXhead(grid, "FILE", "ASC", VNULL, fnams[count]);

		/* set the merged grid bounds to include all the subgrids */
		if( grid->xmin < mgrid->xmin ) mgrid->xmin = grid->xmin;
		if( grid->xmax > mgrid->xmax ) mgrid->xmax = grid->xmax;
		if( grid->ymin < mgrid->ymin ) mgrid->ymin = grid->ymin;
		if( grid->ymax > mgrid->ymax ) mgrid->ymax = grid->ymax;
		if( grid->zmin < mgrid->zmin ) mgrid->zmin = grid->zmin;
		if( grid->zmax > mgrid->zmax ) mgrid->zmax = grid->zmax;

		if( grid->hx > res1 || grid->hy > res2 || grid->hzed > res3 ) warn = 1;
	}

	if(warn){
		Vnm_print(1,"WARNING: The specified output resolution is greater than the\n"
					"		 resolution of the input DX files. Upsampling.......\n");
	}

	/* Cache the bounds for comparison later */
	xminb = mgrid->xmin; yminb = mgrid->ymin; zminb = mgrid->zmin;
	xmaxb = mgrid->xmax; ymaxb = mgrid->ymax; zmaxb = mgrid->zmax;

	/* Adjust the boundaries of the grid to any specified by the user */
	if(xmin != 0.0) mgrid->xmin = xmin;
	if(ymin != 0.0) mgrid->ymin = ymin;
	if(zmin != 0.0) mgrid->zmin = zmin;

	if(xmax != 0.0) mgrid->xmax = xmax;
	if(ymax != 0.0) mgrid->ymax = ymax;
	if(zmax != 0.0) mgrid->zmax = zmax;

	/* Now check the boundaries the user specified (if any)
		to make sure they fit within the original
	 */
	if((mgrid->xmin < xminb) ||
	   (mgrid->ymin < yminb) ||
	   (mgrid->zmin < zminb) ||
	   (mgrid->xmax > xmaxb) ||
	   (mgrid->ymax > ymaxb) ||
	   (mgrid->zmax > zmaxb))
	{
		Vnm_print(1,"\nError: The bounds requested do not fall within the bounds of the specified grid\n"
					"You specified <xmin> <ymin> <zmin> <xmax> <ymax> <zmax>: %lf\t%lf\t%lf\t%lf\t%lf\t%lf\n"
					"The input DX files provided                            : %lf\t%lf\t%lf\t%lf\t%lf\t%lf\n",
					mgrid->xmin,mgrid->ymin,mgrid->zmin,mgrid->xmax,mgrid->ymax,mgrid->zmax,
					xminb,yminb,zminb,xmaxb,ymaxb,zmaxb
				  );
		return 1;
	}

	/* set the grid increment for the merged grid */
	mgrid->nx	= (int)VFLOOR(((mgrid->xmax - mgrid->xmin) / resx) + 1.5);
	mgrid->ny	= (int)VFLOOR(((mgrid->ymax - mgrid->ymin) / resy) + 1.5);
	mgrid->nz	= (int)VFLOOR(((mgrid->zmax - mgrid->zmin) / resz) + 1.5);

	mgrid->hx   = (mgrid->xmax - mgrid->xmin) / (mgrid->nx-1);
	mgrid->hy   = (mgrid->ymax - mgrid->ymin) / (mgrid->ny-1);
	mgrid->hzed = (mgrid->zmax - mgrid->zmin) / (mgrid->nz-1);

	/* print out the dimensions of the merged grid */
	Vnm_print(1, "%s Dimensions of the merged grid\n",snam);
	Vnm_print(1, "%s nx = %d, ny = %d, nz = %d\n",snam,
			  mgrid->nx, mgrid->ny, mgrid->nz);
	Vnm_print(1, "%s hx = %lf, hy = %lf, hz = %lf\n",snam,
			  mgrid->hx, mgrid->hy, mgrid->hzed);
	Vnm_print(1, "%s xmin = %lf, ymin = %lf, zmin = %lf\n",snam,
			  mgrid->xmin, mgrid-> ymin, mgrid->zmin);
	Vnm_print(1, "%s xmax = %lf, ymax = %lf, zmax = %lf\n",snam,
			  mgrid->xmax, mgrid-> ymax, mgrid->zmax);

	if(spec) return 0;

	mem_size = (size_t)mgrid->nx * mgrid->ny * mgrid->nz;
	mgrid->data = (double *) Vmem_malloc(mgrid->mem, mem_size, sizeof(double));
	mgrid->ctordata = 1;

	carray = (short *) Vmem_malloc(VNULL, mem_size, sizeof(short) );

	/* initialize the data of the merged grid with zeros */
	nx = mgrid->nx;
	ny = mgrid->ny;
	nz = mgrid->nz;
	for (i = 0; i < nx; i++) {
		for (j = 0; j < ny; j++) {
			for (k = 0; k < nz; k++) {
				(mgrid->data)[IJK(i,j,k)] = 0.0;
				carray[IJK(i,j,k)] = 0;
			}
		}
	}

	/* ************** MERGE THE GRID FILES **************** */
	Vnm_print(1, "%s Reading and Merging...\n",snam);
	for (count=0; count<numfnams; count++) {
                if (formatin == VDF_DX) {
		        Vgrid_readDX(grid, "FILE", "ASC", VNULL, fnams[count]);
                } else if (formatin == VDF_DXBIN) {
		        Vgrid_readDXBIN(grid, "FILE", "ASC", VNULL, fnams[count]);
                }
		xmin = grid->xmin - grid->hx   - VSMALL;
		ymin = grid->ymin - grid->hy   - VSMALL;
		zmin = grid->zmin - grid->hzed - VSMALL;
		xmax = grid->xmax + grid->hx   + VSMALL;
		ymax = grid->ymax + grid->hy   + VSMALL;
		zmax = grid->zmax + grid->hzed + VSMALL;
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
	Vnm_print(1, "%s Verifying and Smoothing...\n", snam);
	for (i=0; i<nx; i++) {
		for (j=0; j<ny; j++) {
			for (k=0; k<nz; k++) {
				if ( carray[IJK(i,j,k)] >= 1 ) {
					(mgrid->data)[IJK(i,j,k)] /= carray[IJK(i,j,k)];
				} else {
					Vnm_print(2,"%s Warning: Gap in subgrids at point (%g,%g,%g)\n",
							  snam,
							  mgrid->xmin + i*mgrid->hx,
							  mgrid->ymin + j*mgrid->hy,
							  mgrid->zmin + k*mgrid->hzed );
				}
			}
		}
	}

	/* ************** WRITE THE MERGED GRID **************** */
	Vnm_print(1, "%s Writing...\n",snam);
	Vnm_print(0, "%s  Writing merged data to %s...\n",snam,outname);
        if (formatout == VDF_DX) {
	        Vgrid_writeDX(mgrid, "FILE", "ASC", VNULL, outname,"mergedx",VNULL);
        } else if (formatout == VDF_DXBIN) {
	        Vgrid_writeDXBIN(mgrid, "FILE", "ASC", VNULL, outname,"mergedx",VNULL);
        }

	Vmem_free(VNULL,(mgrid->nx*mgrid->ny*mgrid->nz), sizeof(short),
			  (void **)&(carray));
	Vgrid_dtor( &mgrid );

	return 0;
}

/* ///////////////////////////////////////////////////////////////////////////
// Routine:  Vgrid_value2
//
// Author:   Nathan Baker and Stephen Bond
/////////////////////////////////////////////////////////////////////////// */
VPUBLIC int Vgrid_value2(Vgrid *thee, double pt[3], double *value) {

	size_t nx, ny, nz, ihi, jhi, khi, ilo, jlo, klo;
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
	ihi = (size_t)ceil(ifloat);
	jhi = (size_t)ceil(jfloat);
	khi = (size_t)ceil(kfloat);
	ilo = (size_t)(floor(ifloat) < 0 ? 0 : floor(ifloat));
	jlo = (size_t)(floor(jfloat) < 0 ? 0 : floor(jfloat));
	klo = (size_t)(floor(kfloat) < 0 ? 0 : floor(kfloat));

	/* If the point is outside the mesh, push it to the mesh */
	if ( ilo == 0 ) {
		ihi = ilo + 1;
		ifloat = (double)(ilo);
	} else if ( ihi >= nx ) {
		ihi = nx - 1;
		ilo = ihi - 1;
		ifloat = (double)(ihi);
	}
	if ( jlo == 0 ) {
		jhi = jlo + 1;
		jfloat = (double)(jlo);
	} else if ( jhi >= ny ) {
		jhi = ny - 1;
		jlo = jhi - 1;
		jfloat = (double)(jhi);
	}
	if ( klo == 0 ) {
		khi = klo + 1;
		kfloat = (double)(klo);
	} else if ( khi >= nz ) {
		khi = nz - 1;
		klo = khi - 1;
		kfloat = (double)(khi);
	}

	/* See if we're on the mesh */
	/*seems that we don't need to check the values including and after ilo>=0 since they are of type size_t
	 * and always positive and is generating warnings when building with clang. (by Juan Brandi).
	 */
	if ((ihi<nx) && (jhi<ny) && (khi<nz) /*&&
		(ilo>=0) && (jlo>=0) && (klo>=0)*/) {
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


VPRIVATE int Char_parseARGV(int argc, char **argv,
  double *res1, double *res2, double *res3, 
  double *xmin, double *ymin, double *zmin, 
  double *xmax, double *ymax, double *zmax, 
  int* spec, char *outname, char fnams[MAX_INPUT_2][MAX_INPUT_PATH], int *numfnams,
  Vdata_Format *formatin, Vdata_Format *formatout)
{
	int i;
	i = 1;
	*numfnams = 0;
	while( i < argc ) {
		if( argv[i][0] == '-' ) {
			char *opt = argv[i++];
			if (!strcmp(opt,"-r")) {
				*res1 = atof(argv[i++]);
				*res2 = atof(argv[i++]);
				*res3 = atof(argv[i++]);
			} else if (!strcmp(opt,"-b")) {
				*xmin = atof(argv[i++]);
				*ymin = atof(argv[i++]);
				*zmin = atof(argv[i++]);

				*xmax = atof(argv[i++]);
				*ymax = atof(argv[i++]);
				*zmax = atof(argv[i++]);
			} else if (!strcmp(opt,"-o")) {
				strcpy(outname,argv[i++]);
			} else if (!strcmp(opt,"-i")) {
				if (!strcmp(argv[i],"dx")) {
                                        *formatin = VDF_DX;
                                } else if (!strcmp(argv[i],"dxbin")) {
                                        *formatin = VDF_DXBIN;
                                } else {
                                        printf("Unknown -i specification: %s\n",argv[i]);
                                        return 1;
                                }
                                i++;
			} else if (!strcmp(opt,"-t")) {
				if (!strcmp(argv[i],"dx")) {
                                        *formatout = VDF_DX;
                                } else if (!strcmp(argv[i],"dxbin")) {
                                        *formatout = VDF_DXBIN;
                                } else {
                                        printf("Unknown -t specification: %s\n",argv[i]);
                                        return 1;
                                }
                                i++;
			} else if (!strcmp(opt,"-s")) {
				*spec = 1;
			} else if (!strcmp(opt,"-h")) {
				return 1;
			} else {
				printf("Unknown option: %s\n", opt);
				return 1;
			}
		} else {
			strcpy(fnams[*numfnams],argv[i++]);
			(*numfnams)++;
		}
	}

	return 0;
}

