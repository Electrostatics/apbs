--------------------
Conversion utilities
--------------------

^^^^^^
del2dx
^^^^^^

Converts DelPhi-format map files (electrostatic potential, etc.) to APBS OpenDX format.
Found in :file:`tools/mesh`

^^^^^^
dx2mol
^^^^^^

Converts OpenDX format map files to MolMol format.
Found in :file:`tools/mesh`

^^^^^^^
dx2uhbd
^^^^^^^

Converts OpenDX format map files to UHBD format.
Found in :file:`tools/mesh`

----------------------
Benchmarking utilities
----------------------

^^^^^^^^^^^^
uhbd_asc2bin
^^^^^^^^^^^^

Converts UHBD ASCII-format files to binary format.
Found in :file:`tools/mesh`

----------------------------
Setup and analysis utilities
----------------------------

^^^^^^^^
analysis
^^^^^^^^

Calculates various metrics from input scalar data.
Found in :file:`tools/mesh`

^^^^
born
^^^^

Calculate generalized Born forces and energies.
Found in :file:`tools/manip`

^^^^^^^
coulomb
^^^^^^^

Calculate Coulomb forces and energies.
Found in :file:`tools/manip`

.. _dxmath:

^^^^^^
dxmath
^^^^^^

Performs simple arithmetic operations with Cartesian grid data.  
This program takes as input a file with operations specified in a stack-based (RPN) manner.
For example, a command file which adds grid1 and grid2, multiplies the result by 5.3, adds grid4, subtracts 99.3 from the whole thing, and writes the result on grid5 would have the form:

.. code-block:: mathematica
   
   grid1
   grid2 +
   5.3 *
   grid4 +
   99.3 -
   grid5 =

The file names, scalar values, and operations must be separated by tabs, line breaks, or white space.
Comments can be included between the character # and a new line (in the usual shell script fashion).
Found in :file:`tools/mesh`

^^^^^^^^^^^^^^^^^^^^
mergedx and mergedx2
^^^^^^^^^^^^^^^^^^^^

Combine multiple OpenDX files into a single resampled file.
:program:`mergedx2` can perform a number of grid manipulation operations, including:

* Combining multiple OpenDX map files
* Resampling of one or more OpenDX map files (for example to alter the grid spacing of separate OpenDX files for further manipulation)
* Extracting a subregion of an existing OpenDX map file.

Found in :file:`tools/mesh`

^^^^^^
mgmesh
^^^^^^

Prints out acceptable combinations of :doc:`input/elec/nlev` and :doc:`input/elec/dime` for multigrd calculations.
Found in :file:`tools/mesh`

^^^^^^^^^^
multivalue
^^^^^^^^^^

This program evaluates OpenDX scalar data at a series of user-specified points and returns the value of the data at each point.
Found in :file:`tools/mesh`

^^^^^^^^^^
similarity
^^^^^^^^^^

Computes similarity between two scalar grid datasets.
Found in :file:`tools/mesh`

^^^^^^
smooth
^^^^^^

Convolve grid data with various filters.
Found in :file:`tools/mesh`

