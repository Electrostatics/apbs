/* ///////////////////////////////////////////////////////////////////////////
// File:     dx2uhbd.c
//
// Purpose:  Convert OpenDx format potential to UHBD format
//
// Author:   rok, based on dx2mol.c
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

  /*** Variables ***/
  size_t u, i, j, k, nx, ny, nz;
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

  char *title = "dx2uhbd conversion";
  char *usage = "\n\n\
    -----------------------------------------------------------------------\n\
    Converts the OpenDX format of the electrostatic potential \n\
    to the UHBD format.\n\n\
    Usage:  dx2uhbd <file1> <file2.grd> [dx_type]\n\n\
            where file1 is a file in OpenDX format,\n\
            and file2.grd is the file to be written in UHBD format.\n\n\
            The optional argument dx_type specifies the input OpenDX type.\n\
            Acceptable values include\n\
                dx:  standard OpenDX format\n\
                dxbin:  binary OpenDX format\n\
            If the argument is unspecified, the input type is standard OpenDX.\n\
    -----------------------------------------------------------------------\n\
    \n";


  /*** Check Invocation ***/
  Vio_start();
  if (argc != 3 && argc != 4) {
    Vnm_print(2, "\n*** Syntax error: got %d arguments, expected 2 or 3.\n\n",
          argc-1);
    Vnm_print(2,"%s\n", usage);
    return EXIT_FAILURE;
  } else {
    inpath = argv[1];
    outpath = argv[2];

    if (argc == 4) {
        if (!Vstring_strcasecmp(argv[3], "dx")) {
            dx_type = VDF_DX;
        } else if (!Vstring_strcasecmp(argv[3], "dxbin")) {
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
                Vnm_print(2, "\n*** Fatal error while reading from %s as "
                             "binary DX format file\n", inpath);
                return EXIT_FAILURE;
        }
    } else if (dx_type == VDF_DX) {                                                                                 
        /* Read standard DX format file */
        Vnm_print(1, "Reading %s as standard OpenDX format...\n", inpath);
        if(Vgrid_readDX(grid, "FILE", "ASC", VNULL, inpath) != 1) {
                Vnm_print(2, "\n*** Fatal error while reading from %s as "
                             "standard DX format file\n", inpath);
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
  Vnm_tprint(1, "  %d x %d x %d grid\n", nx, ny, nz);
  Vnm_tprint(1, "  (%g, %g, %g) A spacings\n", hx, hy, hzed);
  Vnm_tprint(1, "  (%g, %g, %g) A lower corner\n",
         xmin, ymin, zmin);



  /*** Intialize socket for writing ***/
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

  /*** Write potential data in UHBD format ***/
  /*   sprintf(outpath, "%s.%s", writestem, "grd"); */

  Vnm_tprint(1, "Writting UHBD file ... \n");
  /* Vnm_tprint(1, "%s\n", outpath); */
  Vgrid_writeUHBD(grid, "FILE", "ASC", VNULL, outpath, title,
          grid->data);
  Vgrid_dtor(&grid);


  /*** Close the socket ***/
  Vio_connectFree(sock);
  Vio_dtor(&sock);

  return 0;
}
