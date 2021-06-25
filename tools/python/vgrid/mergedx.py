#!/usr/bin/python3 -O

import sys
import getopt
from .vgrid import (
    startVio,
    Vgrid_ctor,
    Vgrid_readDX,
    Vgrid_value,
    Vgrid_writeDX,
    delete_vgrid,
    null_array,
)

"""
    mergedx.py - Python script for merging dx files

    Written by Todd Dolinsky (todd@ccb.wustl.edu)
    Washington University in St. Louis
"""

__date__ = "January 2005"
__author__ = "Todd Dolinsky"

VSMALL = 1.0e-9
OUT = "mergedgrid.dx"
HEADER = "\n\n\
    ----------------------------------------------------------------------\n\
    Adaptive Poisson-Boltzmann Solver (APBS)\n\
    ----------------------------------------------------------------------\n\
    \n\n"


def IJK(nx, ny, nz, i, j, k):
    """
        Translate a three dimensional point to
        a one dimensional list

        Parameters
            nx:    No. of total gridpoints in x direction (int)
            ny:    No. of total gridpoints in y direction (int)
            nz:    No. of total gridpoints in z direction (int)
            i:     Specific gridpoint in x direction (int)
            j:     Specific gridpoint in y direction (int)
            k:     Specific gridpoint in z direction (int)

        Returns
            value: The one dimensional value matching the
                   three dimensional point
    """
    value = k * nx * ny + j * nx + i
    return value


def createGrid(inputpath, root):
    """
        Create the merged grid by use of an APBS input file and
        the multiple dx files.

        Parameters
            inputpath: The path to the APBS input file (string)
            root:      The root of the name of the multiple dx files,
                       to be completed with <int>.dx (string)
        Returns
            mygrid:    The merged grid object (Vgrid)
    """

    # Initialize some variables

    myaccess = []
    ofrac = [0, 0, 0]
    pdime = [0, 0, 0]
    dime = [0, 0, 0]
    fglen = [0, 0, 0]
    glob = [0, 0, 0]
    mygrid = None
    mydata = None

    # Parse the input file for useful information

    inputfile = open(inputpath)
    while 1:
        line = inputfile.readline()
        if line == "":
            break
        words = line.split()
        if len(words) == 0:
            continue
        if words[0] == "ofrac":
            ofrac[0] = float(words[1])
            ofrac[1] = float(words[1])
            ofrac[2] = float(words[1])
        elif words[0] == "pdime":
            pdime[0] = int(words[1])
            pdime[1] = int(words[2])
            pdime[2] = int(words[3])
        elif words[0] == "dime":
            dime[0] = int(words[1])
            dime[1] = int(words[2])
            dime[2] = int(words[3])
        elif words[0] == "fglen":
            fglen[0] = float(words[1])
            fglen[1] = float(words[2])
            fglen[2] = float(words[3])

    inputfile.close()

    if pdime[0] == 1:
        ofrac[0] = 0
    if pdime[1] == 1:
        ofrac[1] = 0
    if pdime[2] == 1:
        ofrac[2] = 0

    glob[0] = pdime[0] * int(round(dime[0] / (1 + 2 * ofrac[0])))
    glob[1] = pdime[1] * int(round(dime[1] / (1 + 2 * ofrac[1])))
    glob[2] = pdime[2] * int(round(dime[2] / (1 + 2 * ofrac[2])))

    size = pdime[0] * pdime[1] * pdime[2]

    myxmin = None
    myymin = None
    myzmin = None
    myhx = None
    myhy = None
    myhzed = None
    mydata = []

    # Read each dx file

    for i in range(size):
        mins = []
        for j in range(3):
            mins.append(None)
        kp = int(i / (pdime[0] * pdime[1]))
        jp = int((i - kp * pdime[0] * pdime[1]) / pdime[0])
        ip = i - kp * pdime[0] * pdime[1] - jp * pdime[0]
        mins[0] = ip * glob[0] / pdime[0]
        mins[1] = jp * glob[1] / pdime[1]
        mins[2] = kp * glob[2] / pdime[2]

        filename = "%s%i.dx" % (root, i)
        try:
            file = open(filename)
            file.close()
        except IOError:
            print(f"Unable to find file {filename}!")
            sys.exit()

        data = []
        grid = Vgrid_ctor(0, 0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, data)

        print(f"Reading dx file {filename}...")
        Vgrid_readDX(grid, "FILE", "ASC", "", filename)
        grid.xmax = grid.xmin + grid.hx * (grid.nx - 1)
        grid.ymax = grid.ymin + grid.hy * (grid.ny - 1)
        grid.zmax = grid.zmin + grid.hzed * (grid.nz - 1)

        print("\tGrid dimensions: %i %i %i" % (grid.nx, grid.ny, grid.nz))
        print("\tGrid spacing: %.5f %.5f %.5f" % (grid.hx, grid.hy, grid.hzed))
        print(
            "\tGrid lower corner: %.2f %.2f %.2f"
            % (grid.xmin, grid.ymin, grid.zmin,)
        )
        print(
            "\tGrid upper corner: %.2f %.2f %.2f"
            % (grid.xmax, grid.ymax, grid.zmax,)
        )
        print(
            "\tGlobal Gridpoint Minima: %i %i %i\n"
            % (mins[0], mins[1], mins[2],)
        )

        # If this is the first processor, initialize the merged grid

        if i == 0:  # Initialize merged grid
            myhx = fglen[0] / (glob[0] - 1)
            myhy = fglen[1] / (glob[1] - 1)
            myhzed = fglen[2] / (glob[2] - 1)
            myxmin = grid.xmin
            myymin = grid.ymin
            myzmin = grid.zmin
            for j in range(glob[0] * glob[1] * glob[2]):
                mydata.append(0.0)
                myaccess.append(0)

        # If this processor is the last in a given direction, do a sanity check

        if ip == (pdime[0] - 1):
            if glob[0] != (mins[0] + grid.nx):
                print("Error occurred - This grid does not line up globally!")
                print(f"Global x-dim gridpoint size:   {glob[0]}")
                print(f"This grid's maximum gridpoint: {(mins[0] + grid.nx)}")
                sys.exit()
        if jp == (pdime[1] - 1):
            if glob[1] != (mins[1] + grid.ny):
                print("Error occurred - This grid does not line up globally!")
                print(f"Global x-dim gridpoint size:   {glob[1]}")
                print(f"This grid's maximum gridpoint: {(mins[1] + grid.ny)}")
                sys.exit()
        if kp == (pdime[2] - 1):
            if glob[2] != (mins[2] + grid.nz):
                print("Error occurred - This grid does not line up globally!")
                print(f"Global x-dim gridpoint size:   {glob[2]}")
                print(f"This grid's maximum gridpoint: {(mins[2] + grid.nz)}")
                sys.exit()

        # Now add this grid to the merged grid

        for x in range(grid.nx):
            ival = grid.xmin + x * grid.hx
            for y in range(grid.ny):
                jval = grid.ymin + y * grid.hy
                for z in range(grid.nz):
                    kval = grid.zmin + z * grid.hzed
                    inval = 0.0
                    pt = [ival, jval, kval]
                    ret, value = Vgrid_value(grid, pt, inval)
                    if ret:
                        location = IJK(
                            glob[0],
                            glob[1],
                            glob[2],
                            (x + mins[0]),
                            (y + mins[1]),
                            (z + mins[2]),
                        )
                        myaccess[location] += 1
                        mydata[location] = value
                    else:
                        print(f"ival:{ival}, jval:{jval}, kval:{kval}")
                        msg = "Could not find gridpoint "
                        msg += f"{x} {y} {z} in grid {filename}!"
                        print(msg)
                        sys.exit()

        # Delete this grid object

        delete_vgrid(grid)

    # Make sure all values of the grid were accessed

    print("Ensuring all grid points were merged...")
    for i in range(glob[0]):
        for j in range(glob[1]):
            for k in range(glob[2]):
                location = IJK(glob[0], glob[1], glob[2], i, j, k)
                if myaccess[location] == 0:
                    print(f"Error: Found unaccessed gridpoint at {i} {j} {k}!")
                    sys.exit()
                elif myaccess[location] > 1:  # Pt. on multiple grids: Error!
                    msg = "Error: Multiple grids attempted to access gridpoint"
                    msg += f" {i} {j} {k} in the global grid!"
                    print(msg)
                    sys.exit()

    # Make the grid

    mygrid = Vgrid_ctor(
        glob[0],
        glob[1],
        glob[2],
        myhx,
        myhy,
        myhzed,
        myxmin,
        myymin,
        myzmin,
        mydata,
    )

    return mygrid


def resampleGrid(grid, nx, ny, nz):
    """
        Resample the grid to a smaller (less-defined) resolution

        Parameters
            grid:   The merged grid (Vgrid)
            nx:     The number of gridpoints in the x dir (int)
            nx:     The number of gridpoints in the x dir (int)
            nx:     The number of gridpoints in the x dir (int)

        Retrurns
            newgrid: The resampleed merged grid (Vgrid)
    """

    print("Resampling the grid...")

    # Ensure that the new grid size is smaller than the old grid size

    if nx > grid.nx or ny > grid.ny or nz > grid.nz:
        print(f"Error: User specified grid size ({nx} {ny} {nz}) is larger ")
        print(f"than merged grid size ({grid.nx} {grid.ny} {grid.nz})!")
        sys.exit()

    # Get new grid spacing, and create initialized grid

    xmin = grid.xmin
    ymin = grid.ymin
    zmin = grid.zmin
    xmax = grid.xmin + grid.hx * (grid.nx - 1)
    ymax = grid.ymin + grid.hy * (grid.ny - 1)
    zmax = grid.zmin + grid.hzed * (grid.nz - 1)
    hxnew = (xmax - xmin) / (nx - 1)
    hynew = (ymax - ymin) / (ny - 1)
    hznew = (zmax - zmin) / (nz - 1)
    mydata = []
    for i in range(nx * ny * nz):
        mydata.append(0.0)

    # Populate the new grid

    for x in range(nx):
        ival = xmin + x * hxnew
        for y in range(ny):
            jval = ymin + y * hynew
            for z in range(nz):
                kval = zmin + z * hznew
                pt = [ival, jval, kval]
                inval = 0.0
                ret, value = Vgrid_value(grid, pt, inval)
                if ret:
                    location = IJK(nx, ny, nz, x, y, z)
                    if value < VSMALL and value > 0:
                        value = 0.0
                    mydata[location] = value
                else:
                    print(f"Could not find gridpoint {x} {y} {z} in grid!")
                    sys.exit()

    # Delete the old grid
    delete_vgrid(grid)

    # Make the new grid
    newgrid = Vgrid_ctor(
        nx, ny, nz, hxnew, hynew, hznew, xmin, ymin, zmin, mydata
    )
    return newgrid


def printGrid(mygrid, outpath):
    """
        Print the merged grid using the Vgrid_writeDX command

        Parameters
            mygrid:  The merged grid (Vgrid)
            outpath: The output path for the new .dx file (string)
    """
    print(f"Writing output to {outpath}...")
    title = "Merged Grid from mergedx.py"
    Vgrid_writeDX(mygrid, "FILE", "ASC", "", outpath, title, null_array())


def usage():
    """
        Print usage information
    """
    val = f"{HEADER}"
    val += """
mergedx.py

This module merges multiple dx files generated from parallel
APBS runs into one merged dx file. Users may also resample the
grid size if desired.  Default output is written to mergedgrid.dx
Usage: mergedx.py [options] <input-file> <dx-stem>

    Required Arguments:
        <input-file>   : The path to an APBS input file used to
                         generate the dx file.  If the APBS run was
                         asynchronous, any of the input files may be used
        <dx-stem>      : The stem of the dx filenames.  This script will
                         add the digit and the .dx extension -  note that
                         the dx files MUST be trailed by the form *.dx

                         Example: given dx files 2PHKB-PE0.dx and 2PHKB-PE1.dx,
                                  the stem would be 2PHKB-PE

   Optional Arguments:
        --help   (-h)  : Display the usage information
        --out=<outpath>: Save merged dx file into filename <outpath>
        --nx=<xsize>   : Resample to the <xsize> gridpoints in the x direction
        --ny=<ysize>   : Resample to the <ysize> gridpoints in the z direction
        --nz=<zsize>   : Resample to the <zsize> gridpoints in the z direction
                         Note: If resampling, nx, ny, and nz all must be
                         specified.
"""
    sys.stderr.write(val)
    sys.exit()


def main():
    """
        The main driver for the mergedx script
    """
    shortOptlist = "h"
    longOptlist = ["help", "out=", "nx=", "ny=", "nz="]
    try:
        opts, args = getopt.getopt(sys.argv[1:], shortOptlist, longOptlist)
    except getopt.GetoptError as details:
        sys.stderr.write(f"GetoptError:  {details}\n")
        usage()

    outpath = OUT
    nx = None
    ny = None
    nz = None
    resample = 0
    for o, a in opts:
        if o in ("-h", "--help"):
            usage()
            sys.exit()
        elif o == "--out":
            outpath = a
        elif o == "--nx":
            nx = int(a)
        elif o == "--ny":
            ny = int(a)
        elif o == "--nz":
            nz = int(a)

    if nx is not None and ny is not None and nz is not None:
        resample = 1
    elif nx is None and ny is None and nz is None:
        pass
    else:
        print("\nYou must enter either none or all values for nx, ny, and nz!")
        usage()
        sys.exit()

    argid = 1
    if outpath != OUT:
        argid += 1
    if resample == 1:
        argid += 3

    try:
        inputpath = sys.argv[argid]
        root = sys.argv[argid + 1]
    except IndexError:
        print("\nImproper number of arguments!")
        usage()
        sys.exit()

    startVio()

    mygrid = createGrid(inputpath, root)
    if resample:
        mygrid = resampleGrid(mygrid, nx, ny, nz)
    printGrid(mygrid, outpath)

    # If we're outputting back to stdout, delete the grid
    delete_vgrid(mygrid)


if __name__ == "__main__":
    main()
