/**
*  @file    value.c
 *  @author  Nathan Baker
 *  @brief   Get information about solution at a point
 *  @version $Id$
 */

#include "apbs.h"

VEMBED(rcsid="$Id$")

int usage(int rc) {

    char *usage = "\n\n\
----------------------------------------------------------------------\n\
    This driver program reads in data and prints solution information at a\n\
    point.  It is invoked as:\n\
    value <x> <y> <z> <file.dx>\n\n\
    where <x>, <y>, and <z> are points and <file.dx> is an OpenDX-format\n\
    file.\n\
----------------------------------------------------------------------\n\n";

    Vnm_print(2, usage);

    exit(rc);

    return 0;
}

int main(int argc, char **argv) {

    Vgrid *grid;
    int inorm;
    char *path;
    double pt[3], val, grad[3];

    /* *************** CHECK INVOCATION ******************* */
    Vio_start();
    Vnm_redirect(1);
    Vnm_print(1, "\n");
    if (argc != 5) {
        Vnm_print(2, "Error -- got %d arguments, expected 5.\n", argc);
        usage(2);
    }
    sscanf(argv[1], "%lf", &(pt[0]));
    sscanf(argv[2], "%lf", &(pt[1]));
    sscanf(argv[3], "%lf", &(pt[2]));
    path = argv[4];

    /* *************** READ DATA ******************* */
    Vnm_print(1, "Reading data from %s...\n", path);
    grid = Vgrid_ctor(0, 0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, VNULL);
    if (!Vgrid_readDX(grid, "FILE", "ASC", VNULL, path)) {
        Vnm_print(2, "main:  Problem reading OpenDX-format grid from %s\n",
                  path);
        return 2;
    }

    /* *************** READ DATA ******************* */
    Vnm_print(1, "\nData at (%g, %g, %g):\n", pt[0], pt[1], pt[2]);
    if (Vgrid_value(grid, pt, &val)) {
        Vnm_print(1, "Value = %1.12E kT/e\n", val);
    } else  Vnm_print(1, "Unable to get value.\n");
    if (Vgrid_gradient(grid, pt, grad)) {
        Vnm_print(1, "Gradient = (%1.12E, %1.12E, %1.12E) kT/e/A\n",
                  grad[0], grad[1], grad[2]);
    } else  Vnm_print(1, "Unable to get gradient.\n");
    /*
    if (Vgrid_curvature(grid, pt, 0, &val)) {
        Vnm_print(1, "Reduced maximal curvature = %1.12E kT/e/A/A\n", val);
    } else Vnm_print(1, "Unable to get curvature.\n");
    if (Vgrid_curvature(grid, pt, 1, &val)) {
        Vnm_print(1, "Mean curvature (Laplace) = %1.12E kT/e/A/A\n", val);
    } else Vnm_print(1, "Unable to get curvature.\n");
    if (Vgrid_curvature(grid, pt, 2, &val)) {
        Vnm_print(1, "Gauss curvature = %1.12E kT/e/A/A\n", val);
    } else Vnm_print(1, "Unable to get curvature.\n");
    if (Vgrid_curvature(grid, pt, 3, &val)) {
        Vnm_print(1, "True maximal curvature = %1.12E kT/e/A/A\n", val);
    } else Vnm_print(1, "Unable to get curvature.\n");
    */

    Vnm_print(1, "\n");
    return 0;

}
