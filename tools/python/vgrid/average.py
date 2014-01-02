from vgrid import *
import sys
import math
from sys import stdout, stderr

"""
    average.py - An example script for interfacing Python with APBS
              Vgrid routines
"""

header = "\n\n\
    ----------------------------------------------------------------------\n\
    Adaptive Poisson-Boltzmann Solver (APBS)\n\
    ----------------------------------------------------------------------\n\
    \n\n"
    

usage = "python[2] average.py file.dx\n";

def main():

    # *************** CHECK INVOCATION *******************
    
    startVio()

    if len(sys.argv) != 2:
        stderr.write("\n*** Syntax error: got %d arguments, expected 2.\n\n" % len(sys.argv))
        stderr.write("%s\n" % usage)
        sys.exit(2)

    else:
        inpath = sys.argv[1]

    # *************** APBS INITIALIZATION ******************* 

    stdout.write(header)
    data = []

    stdout.write("main:  Reading data from %s... \n" % inpath)
    grid = Vgrid_ctor(0, 0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, data)
    Vgrid_readDX(grid, "FILE", "ASC", "", inpath)

    nx = grid.nx
    ny = grid.ny
    nz = grid.nz
    hx = grid.hx
    hy = grid.hy
    hzed = grid.hzed
    xmin = grid.xmin
    ymin = grid.ymin
    zmin = grid.zmin

    stdout.write("#     nx = %d, ny = %d, nz = %d\n" % (nx, ny, nz))
    stdout.write("#     hx = %g, hy = %g, hz = %g\n" % (hx, hy, hzed))
    stdout.write("#     xmin = %g, ymin = %g, zmin = %g\n" % (xmin, ymin, zmin))
    # *************** SETTINGS ********************

    xcentAVG = 112.160
    ycentAVG = 63.5
    zcentAVG = 137.245
    xlenAVG = 70.0
    zlenAVG = 70.0
    ylenAVG = hy*(ny-1)

    # *************** AVERAGE **********************

    xminAVG = xcentAVG - 0.5*xlenAVG
    xmaxAVG = xcentAVG + 0.5*xlenAVG
    yminAVG = ycentAVG - 0.5*ylenAVG
    ymaxAVG = ycentAVG + 0.5*ylenAVG
    zminAVG = zcentAVG - 0.5*zlenAVG
    zmaxAVG = zcentAVG + 0.5*zlenAVG
    imin = int(math.floor((xminAVG - xmin)/hx))
    imin = max(imin, 0)
    jmin = int(math.floor((yminAVG - ymin)/hy))
    jmin = max(jmin, 0)
    kmin = int(math.floor((zminAVG - zmin)/hzed))
    kmin = max(kmin, 0)
    imax = int(math.ceil((xmaxAVG - xmin)/hx))
    imax = min(imax, nx-1)
    jmax = int(math.ceil((ymaxAVG - ymin)/hy))
    jmax = min(jmax, ny-1)
    kmax = int(math.ceil((zmaxAVG - zmin)/hzed))
    kmax = min(kmax, nz-1)

    stdout.write("#  \tY POS\t\tAVERAGE\n")
    for j in range(jmin, jmax):
        avg = 0.0
        navg = 0
        for k in range(kmin, kmax):
            for i in range(imin, imax):
                pt = [i,j,k]
                val = 0.0
                ret, value = Vgrid_value(grid, pt, val)
                if ret:
                    avg = avg + value
                    navg = navg + 1

        if navg != 0:
            avg = avg/navg
            stdout.write("   \t%e\t\t%e\n" % ((hy*j + ymin), avg))
        else:
            stdout.write("   \t%e\t\t%s\n" % ((hy*j + ymin), "nan"))

if __name__ == "__main__": main()
