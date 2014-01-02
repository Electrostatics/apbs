from vgrid import *
import sys
from sys import stdout, stderr

"""
    read.py - An example script for interfacing Python with APBS
              Vgrid routines
"""
header = "\n\n\
    ----------------------------------------------------------------------\n\
    Adaptive Poisson-Boltzmann Solver (APBS)\n\
    ----------------------------------------------------------------------\n\
    \n\n"
    
usage = "python[2] read.py file.dx\n";

NMAX = 5

def main():

    inpath = ""
    value = 0.0
    data = []
    
    startVio()

    if len(sys.argv) != 2:
        stderr.write("\n*** Syntax error: got %d arguments, expected 2.\n\n" % len(sys.argv))
        stderr.write("%s\n" % usage)
        sys.exit(2)

    else:
        inpath = sys.argv[1]

    stdout.write(header)
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

    stdout.write("main:     nx = %d, ny = %d, nz = %d\n" % (nx, ny, nz))
    stdout.write("main:     hx = %g, hy = %g, hz = %g\n" % (hx, hy, hzed))
    stdout.write("main:     xmin = %g, ymin = %g, zmin = %g\n" % (xmin, ymin, zmin))

    # Read off some values

    stdout.write("main:  Moving along x-axis...\n")

    for i in range(nx):
        inval = 0.0
        pt = [xmin + i*hx, ymin + 0.5*(ny-1)*hy, zmin + 0.5*(nz-1)*hzed]
        ret, value = Vgrid_value(grid, pt, inval)
        if ret:
            stdout.write("main: u(%g, %g, %g) = %g\n" % (pt[0], pt[1], pt[2], value))

    # Integrate

    stdout.write("main:  Integrating...\n")
    sum = 0
    for i in range(nx):
        for j in range(ny):
            for k in range(nz):
                inval = 0.0
                pt = [xmin + i*hx, ymin + j*hy, zmin + k*hzed]
                ret, value = Vgrid_value(grid, pt, inval)
                if ret:
                    sum = sum + value
    
    stdout.write("main:  Integral over grid = %1.12E\n" % (sum*hx*hy*hzed))

    grad = [0.0,0.0,0.0]
    Vgrid_gradient(grid, pt, grad)

if __name__ == "__main__": main()
