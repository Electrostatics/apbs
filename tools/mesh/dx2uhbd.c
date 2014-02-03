/* ///////////////////////////////////////////////////////////////////////////
// File:     dx2uhbd.c
//
// Purpose:  Convert OpenDx format potential to UHBD format
//
// Author:   rok, based on dx2mol.c
//
// rcsid="$Id$"
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
  char *title = "dx2uhbd conversion";
  char *usage = "\n\n\
    -----------------------------------------------------------------------\n\
    Converts the OpenDX format of the electrostatic potential \n\
    to the UHBD format.\n\n\
    Usage:  dx2uhbd file1.dx file2.grd\n\n\
            where file1.dx is a file in OpenDX format and file2.grd is the\n\
            file to be written in UHBD format.\n\
    -----------------------------------------------------------------------\n\
    \n";


  /*** Check Invocation ***/
  Vio_start();
  if (argc != 3) {
    Vnm_print(2, "\n*** Syntax error: got %d arguments, expected 2.\n\n",
          argc-1);
    Vnm_print(2,"%s\n", usage);
    return -1;
  } else {
    inpath = argv[1];
    outpath = argv[2];
  }

  /*** Read DX format file ***/
  grid = Vgrid_ctor(0, 0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, VNULL);
  Vnm_tprint(1, "Reading DX file ... \n");
  if(Vgrid_readDX(grid, "FILE", "ASC", VNULL, inpath) != 1) {
    Vnm_tprint( 2, "Fatal error while reading from %s\n",
        inpath);
    return 0;
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
