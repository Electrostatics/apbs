/*
 *  del2dx.c
 *  apbs
 *
 *  Created by David Gohara on 3/17/10.
 *  Copyright 2010 Washington University. All rights reserved.
 * 
 *  Last updated by Leighton Wilson on 08/29/2016:
 *  Added ability to output binary DX files
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <apbs.h>

char *usage = "\n\n\
-----------------------------------------------------------------------\n\
del2dx\n\
\n\
For converting the DelPhi format of the electrostatic potential to the\n\
OpenDX format.\n\
\n\
Usage:  del2dx <delphi_file> <opendx_file> [outputformat]\n\n\
where delphi_file is a file in DelPhi format,\n\
and opendx_file is the file to be written in OpenDX format.\n\n\
The optional argument outputformat specifies the output OpenDX format.\n\
Acceptable values include\n\
       dx:  standard OpenDX format\n\
       dxbin:  binary OpenDX format\n\
If the argument is not specified, the output format is standard OpenDX.\n\
-----------------------------------------------------------------------\n\
\n";

int main(int argc, char **argv) {
	
	int i,j,k,index;
	char *inpath = NULL;
        char *outpath = NULL;
	char buffer[1024];
        Vdata_Format format;
	
	int igrid, tot_grid, icol;
	float val, xmax, scale, oldmid[3], temp, xdata, range, extent, origin[3], delta[3];
	float *data;
	
	FILE * pfile = NULL;
	
    if (!(argc == 3 || argc == 4)) {
        printf("\n*** Syntax error: got %d arguments, expected 2 or 3.\n\n",argc-1);
        printf("%s\n", usage);
        return EXIT_FAILURE;
    } else {
        inpath = argv[1];
        outpath = argv[2];

        if (argc == 4) {
            if (Vstring_strcasecmp(argv[3], "dx")) {
                format = VDF_DX;
            } else if (Vstring_strcasecmp(argv[3], "dxbin")) {
                format = VDF_DXBIN;
            } else {
                printf("\n*** Argument error: format must be 'dx' or 'dxbin'.\n\n");
                return EXIT_FAILURE;
            }
        } else {
            format = VDF_DX;
        }
    }
	
	pfile = fopen(inpath, "r+b");
	
	//Get the grid length (all dimensions are the same)
	fseek(pfile, sizeof(int) * -1, SEEK_END);
	fread(&igrid, 1, sizeof(int), pfile);
	tot_grid = igrid*igrid*igrid;
	
	data = (float *)calloc(tot_grid, sizeof(float));
	
	rewind(pfile);
	
	//Skip the first 20 characters, they simply say
	// "now starting phimap "
	fseek(pfile, sizeof(char) * 20, SEEK_CUR);
	
	//get the map file time: potential or concentrat
	memset(buffer, 0, 1024);
	fread(buffer, 1, sizeof(char) * 10, pfile);
	printf("DelPhi %s map grid dimensions are: %i %i %i\n",
		   buffer,igrid,igrid,igrid);
	
	//Now skip ahead 60 characters to the data set.
	fseek(pfile, sizeof(char) * 60, SEEK_CUR);
	
	//Read in the actual data (float)
	fread(data, tot_grid, sizeof(float), pfile);
	
	//Skip over the text " end of phimap "
	fseek(pfile, sizeof(char) * 16, SEEK_CUR);
	
	//Get the scale
	fread(&scale, 1, sizeof(float), pfile);
	
	//Get the old midpoints?
	fread(oldmid, 3, sizeof(float), pfile);
	
	printf("The DelPhi %s map scale is: %f\n" \
		   "Old midpoint values: %f %f %f\n",
		   buffer,scale,oldmid[0],oldmid[1],oldmid[2]);
	
	fclose(pfile);
	
	//Now calculate some numbers needed for writing the DX header
	temp = 0.;
	xmax = 0.;
	for(i=0;i<3;i++){
		temp = fabsf(oldmid[i]);
		xmax = (xmax < temp) ? temp : xmax;
	}
	
	range = ((float)igrid - 1.) / (2. * scale);
	extent = range + xmax;
	
	for(i=0;i<3;i++) origin[i] = oldmid[i] - range;
	for(i=0;i<3;i++) delta[i] = (range * 2.) / (float)igrid;
	
	//Now write the OpenDX formatted file
	pfile = fopen(outpath, "w");
	
	fprintf(pfile,
		   "# Data from del2dx (APBS 1.3)\n"	\
		   "# \n"							\
		   "# POTENTIAL (kT/e)\n"			\
		   "# \n"							\
		   "object 1 class gridpositions counts %i %i %i\n"	\
		   "origin %1.6e %1.6e %1.6e\n"	\
		   "delta %1.6e 0.000000e+00 0.000000e+00\n"		\
		   "delta 0.000000e+00 %1.6e 0.000000e+00\n"		\
		   "delta 0.000000e+00 0.000000e+00 %1.6e\n"		\
		   "object 2 class gridconnections counts %i %i %i\n"\
		   "object 3 class array type double rank 0 items %i data follows\n",
			igrid,igrid,igrid,
			origin[0],origin[1],origin[2],
			delta[0],delta[1],delta[2],
			igrid,igrid,igrid,tot_grid);
	
	//For the moment I'm assuming the data for DelPhi is row major
	//Write out the data
	icol = 0;
        if (format == VDF_DX) {
	    for (i=0; i<igrid; i++) {
	        for (j=0; j<igrid; j++) { 
		    for (k=0; k<igrid; k++) {
		        index = k*(igrid)*(igrid)+j*(igrid)+i;
			fprintf(pfile, "%12.6e ", data[index]);
			icol++;
			if (icol == 3) {
			    icol = 0;
			    fprintf(pfile, "\n");
			}
		    }
		}
	    }
        } else if (format == VDF_DXBIN) {
	    for (i=0; i<igrid; i++) {
	        for (j=0; j<igrid; j++) { 
		    for (k=0; k<igrid; k++) {
		        index = k*(igrid)*(igrid)+j*(igrid)+i;
                        fwrite(&(data)[index], sizeof(double), 1, pfile);
			icol++;
			if (icol == 3) {
			    icol = 0;
			}
		    }
		}
	    }
        } else {
            printf("\n*** Error: output format (format) incorrectly defined.\n\n");
            return EXIT_FAILURE;
        }


	if (icol != 0) fprintf(pfile, "\n");
	fprintf(pfile, "attribute \"dep\" string \"positions\"\n" \
			"object \"regular positions regular connections\" class field\n" \
			"component \"positions\" value 1\n" \
			"component \"connections\" value 2\n" \
			"component \"data\" value 3\n");
	
	free(data);
	
	return 0;
}
