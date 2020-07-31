.. _opendx:

OpenDX scalar data format
=========================

We output most discretized scalar data (e.g., potential, accessibility, etc.) from APBS in the data format used by the OpenDX software package.
The OpenDX data format is very flexible; the following sections describe the application of this format for APBS multigrid and finite element datasets.

The multigrid data format has the following form:

.. code-block:: bash

   object 1 class gridpositions counts nx ny nz
   origin xmin ymin zmin
   delta hx 0.0 0.0
   delta 0.0 hy 0.0 
   delta 0.0 0.0 hz
   object 2 class gridconnections counts nx ny nz
   object 3 class array type double rank 0 items n data follows
   u(0,0,0) u(0,0,1) u(0,0,2)
   ...
   u(0,0,nz-3) u(0,0,nz-2) u(0,0,nz-1)
   u(0,1,0) u(0,1,1) u(0,1,2)
   ...
   u(0,1,nz-3) u(0,1,nz-2) u(0,1,nz-1)
   ...
   u(0,ny-1,nz-3) u(0,ny-1,nz-2) u(0,ny-1,nz-1)
   u(1,0,0) u(1,0,1) u(1,0,2)
   ...
   attribute "dep" string "positions"
   object "regular positions regular connections" class field
   component "positions" value 1
   component "connections" value 2
   component "data" value 3`

The variables in this format include:

``nx ny nz``
  The number of grid points in the x-, y-, and z-directions

``xmin ymin zmin``
  The coordinates of the grid lower corner

``hx hy hz``
  The grid spacings in the x-, y-, and z-directions.

``n``
  The total number of grid points; :math:`n = nx * ny * nz`

``u(*,*,*)``
  The data values, ordered with the z-index increasing most quickly, followed by the y-index, and then the x-index.

For finite element solutions, the OpenDX format takes the following form:

.. code-block:: bash

   object 1 class array type float rank 1 shape 3 items N
   v1x v1y v1z
   v2x v2y v2z
   ...
   vNx vNy vNz
   object 2 class array type int rank 1 shape 4 items M
   s1a s1b s1c s1d
   s2a s2b s2c s2d
   ...
   sMa sMb sMc sMd
   attribute "element type" string "tetrahedra"
   object 3 class array type float rank 0 items N
   u1
   u2
   ...
   uN
   attribute "dep" string "positions"
   object "irregular positions irregular connections" class field
   component "positions" value 1
   component "connections" value 2
   component "data" value 3
   end

where the variables in this format are:

``N``
  Number of vertices

``vix viy viz``
  Coordinates of vertex i

``M``
  Number of simplices

``sia sib sic sid``
  IDs of vertices in simplex i

``ui``
  Data value associated with vertex i
