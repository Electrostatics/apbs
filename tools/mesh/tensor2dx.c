/*
 *	del2dx.c
 *	apbs
 *
 *	Created by David Gohara on 3/17/10.
 *	Copyright 2010 Washington University. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

char *usage = "\n\n\
-----------------------------------------------------------------------\n\
tensor2dx\n\
\n\
For converting the a tensor (plotkin) format file to the\n\
OpenDX format.\n\
\n\
Usage:	tensor2dx <x-gpoints> <y-gpoints> <z-gpoints> <originfile> <datafile> <output file>\n\
{xyz}-gpoints are the number of grid points in the x,y,z direction\n\
originfile is the file containing origin and grid spacing information\n\
datafile is the file with the data to use at each grid point\n\
\n\
NOTE: This program only handles isotropic tensor files at the moment.\n\
\n\
-----------------------------------------------------------------------\n\
\n";

int main(int argc, char **argv) {
	
	int i,j,k,index,icol;
	int nx,ny,nz;
	int lcount;
	int itmp[3];
	
	double origin_xyz[3];
	double gspace[3];
	double datapt[3];
	
	double tmp[3];
	
	char *origin = NULL;
	char *data = NULL;
	
	char *outpath = NULL;
	
	char buffer[1024];
	
	FILE * pfile1 = NULL;
	FILE * pfile2 = NULL;
	
	FILE * pfile3 = NULL;
	
	if (argc != 7) {
		printf("\n*** Syntax error: got %d arguments, expected 6.\n\n",argc);
		printf("%s\n", usage);
		return -1;
	} else {
		nx = atoi(argv[1]);
		ny = atoi(argv[2]);
		nz = atoi(argv[3]);
		
		origin = argv[4];
		data = argv[5];
		outpath = argv[6];
	}
	
	pfile1 = fopen(origin,"r");
	pfile2 = fopen(data,"r");
	
	//Get the origin and grid spacing information
	fscanf(pfile1,"%lf %lf %lf",&origin_xyz[0],&origin_xyz[1],&origin_xyz[2]);
	fscanf(pfile1,"%lf %lf %lf",&gspace[0],&gspace[1],&gspace[2]);
	fclose(pfile1);
	
	//Check the line count of the file
	lcount = 0;
	while(fgets(buffer,1024,pfile2) != NULL) lcount++;
	fseek(pfile2, 0L, SEEK_SET);
	
	//Verify the line count matches the information specified for the grid spacing
	if((lcount/4) != nx*ny*nz){
		printf("\n*** %i data points specified, only %i data points read\n\n",nx*ny*nz,lcount);
		printf("%s\n", usage);
		return -1;
	}
	
	//Now write the OpenDX formatted file
	pfile3 = fopen(outpath, "w");
	
	fprintf(pfile3,
		   "# Data from tensor2dx (APBS)\n"	\
		   "# \n"							\
		   "# \n"							\
		   "object 1 class gridpositions counts %i %i %i\n" \
		   "origin %1.6e %1.6e %1.6e\n" \
		   "delta %1.6e 0.000000e+00 0.000000e+00\n"		\
		   "delta 0.000000e+00 %1.6e 0.000000e+00\n"		\
		   "delta 0.000000e+00 0.000000e+00 %1.6e\n"		\
		   "object 2 class gridconnections counts %i %i %i\n"\
		   "object 3 class array type double rank 0 items %i data follows\n",
			nx,ny,nz,
			origin_xyz[0],origin_xyz[1],origin_xyz[2],
			gspace[0],gspace[1],gspace[2],
			nx,ny,nz,nx*ny*nz);
	
	//For the moment I'm assuming the data for the tensor file is row major
	//Write out the data
	icol = 0;
	for (i=0; i<nx*ny*nz; i++){
		fscanf(pfile2,"%i %i %i",&itmp[0],&itmp[1],&itmp[2]);
		fscanf(pfile2,"%lf %lf %lf",&datapt[0],&tmp[1],&tmp[2]);
		fscanf(pfile2,"%lf %lf %lf",&tmp[0],&datapt[1],&tmp[2]);
		fscanf(pfile2,"%lf %lf %lf",&tmp[0],&tmp[1],&datapt[2]);
		
		//TODO: We write out the point for datapt[0], because,
		//	    we only deal with isotropic tensors at the moment.
		//	    we'd need to change this code to handle anisotropic
		//	    tensors in the future.
		fprintf(pfile3, "%12.6e ",datapt[0]);
		icol++;
		if (icol == 3) {
			icol = 0;
			fprintf(pfile3, "\n");
		}
	}

	if (icol != 0) fprintf(pfile3, "\n");
	fprintf(pfile3, "attribute \"dep\" string \"positions\"\n" \
			"object \"regular positions regular connections\" class field\n" \
			"component \"positions\" value 1\n" \
			"component \"connections\" value 2\n" \
			"component \"data\" value 3\n");
	
	fclose(pfile2);
	fclose(pfile3);
	
	return 0;
}
