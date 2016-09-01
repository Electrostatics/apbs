
/* ///////////////////////////////////////////////////////////////////////////
// File:     dx2mol.c
//
// Purpose:  Convert OpenDx format potential to MolMol format
//
// Author:   Jung-Hsin Lin (bits added/modified by Nathan Baker)
//           Additional changes by Fred Damberger
//           Additional changes by Leighton Wilson
//
// rcsid="$Id$"
//
//                                                                                           
// Last update: 08/29/2016 by Leighton Wilson
// Description: Added ability to read in binary DX files as input
//
/////////////////////////////////////////////////////////////////////////// */

#include "apbs.h"

VEMBED(rcsid="$Id$")

int main(int argc, char **argv) {

    /* *************** VARIABLES ******************* */
    int u, i, j, k, nx, ny, nz;
    int n;
    double avg;
    double hy, hx, hzed, xmin, ymin, zmin;
    Vio *sock;
    Vgrid *grid;
    char *inpath = VNULL;
    char *outpath = VNULL;
    char *iodev = "FILE";
    char *iofmt = "ASC";
    char *thost = VNULL;

    Vdata_Format dx_type;

    char *usage = "\n\n\
    -----------------------------------------------------------------------\n\
    dx2mol (Contributed by Jung-Hsin Lin)\n\
    \n\
    For converting the OpenDX format of the electrostatic potential to the\n\
    MOLMOL format. MOLMOL is a popular free molecular display program\n\
    (http://www.mol.biol.ethz.ch/wuthrich/software/molmol/).\n\
    \n\
    Usage:  dx2mol <file1> <file2.pot> [dx_type]\n\n\
            where file1 is a file in OpenDX format,\n\
            and file2.pot is the file to be written in MOLMOL format.\n\n\
            The optional argument dx_type specifies the input OpenDX type.\n\
            Acceptable values include\n\
                dx:  standard OpenDX format\n\
                dxbin:  binary OpenDX format\n\
            If the argument is unspecified, the output type is standard OpenDX.\n\
    -----------------------------------------------------------------------\n\
    \n";


    /* *************** CHECK INVOCATION ******************* */
    Vio_start();
    if (argc != 3 && argc != 4) {
        Vnm_print(2, "\n*** Syntax error: got %d arguments, expected 2 or 3.\n\n",argc-1);
        Vnm_print(2,"%s\n", usage);
        return EXIT_FAILURE;
    } else {
        inpath = argv[1];
        outpath = argv[2];

        if (argc == 4) {
            if (Vstring_strcasecmp(argv[3], "dx")) {
                dx_type = VDF_DX;
            } else if (Vstring_strcasecmp(argv[3], "dxbin")) {
                dx_type = VDF_DXBIN;
            } else {
                Vnm_print(2, "\n*** Argument error: dx_type must be 'dx' or 'dxbin'.\n\n");
                return EXIT_FAILURE;
            }
        } else {
            dx_type = VDF_DX;
        }
    }

    /* Read DX format file */
    grid = Vgrid_ctor(0, 0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, VNULL);

    if (dx_type == VDF_DXBIN) {                                                 
        /* Read binary DX format file */
        Vnm_print(1, "Reading %s as binary OpenDX format...\n", inpath);
        if(Vgrid_readDXBIN(grid, "FILE", "ASC", VNULL, inpath) != 1) {
                Vnm_print(2, "\n*** Fatal error while reading from %s as\
                              binary DX format file\n", inpath);
                return EXIT_FAILURE;
        }
    } else if (dx_type == VDF_DX) {
        /* Read standard DX format file */
        Vnm_print(1, "Reading %s as standard OpenDX format...\n", inpath);
        if(Vgrid_readDX(grid, "FILE", "ASC", VNULL, inpath) != 1) {
                Vnm_print(2, "\n*** Fatal error while reading from %s as\
                              standard DX format file\n", inpath);
                return EXIT_FAILURE;
        }
    } else {
        Vnm_print(2, "\n*** Error: dx_type incorrectly specified.\n\n");
        return EXIT_FAILURE;
    }

    nx = grid->nx;
    ny = grid->ny;
    nz = grid->nz;
    hx = grid->hx;
    hy = grid->hy;
    hzed = grid->hzed;
    xmin = grid->xmin;
    ymin = grid->ymin;
    zmin = grid->zmin;

    /* Intialize socket for writing */
    sock = Vio_ctor(iodev,iofmt,thost, outpath, "w");
    if (sock == VNULL) {
        Vnm_print(2, "Problem opening virtual socket %s\n",
          outpath);
        return -1;
    }
    if (Vio_connect(sock, 0) < 0) {
        Vnm_print(2, "Problem connecting virtual socket %s\n",
          outpath);
        return -1;
    }

    /* Write out data */
    Vio_printf(sock, "%8.3f %4d %5.3f\n",xmin,nx,hx);
    Vio_printf(sock, "%8.3f %4d %5.3f\n",ymin,ny,hy);
    Vio_printf(sock, "%8.3f %4d %5.3f\n",zmin,nz,hzed);

    n = 0;
    for (k=0; k<nz; k++) {
        for (j=0; j<ny; j++) {
            for (i=0; i<nz; i++) {
                n++;
                u = nx*ny*k + nx*j + i;
                Vio_printf(sock, "%10.3e ", grid->data[u]);
                if (n == 10) {
                    Vio_printf(sock, "\n");
                    n = 0;
                }
            }
        }
    }

    /* Close off the socket */
    Vio_connectFree(sock);
    Vio_dtor(&sock);

    return 0;
}
