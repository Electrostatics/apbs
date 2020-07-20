.. _pdime:

pdime
=====

Specify the processor array to be used in a parallel focusing (:ref:`mgpara`) calculation.
The syntax is:

.. code-block:: bash
   
   pdime {npx npy npz}

where ``npx npy npz`` are the integer number of processors to be used in the x-, y- and z-directions of the system.
The product ``npx × npy × npz`` should be less than or equal to the total number of processors with which APBS was invoked (usually via mpirun).
If more processors are provided at invocation than actually used during the run, the extra processors are not used in the calculation.
The processors are tiled across the domain in a Cartesian fashion with a specified amount of overlap (see :ref:`ofrac`) between each processor to ensure continuity of the solution.
Each processor's subdomain will contain the number of grid points specified by the dime keyword.
For broad spatial support of the splines, every charge included in partition needs to be at least 1 grid space (:ref:`chgm` ``spl0``), 2 grid spaces (:ref:`chgm` ``spl2``), or 3 grid spaces (:ref:`chgm` ``spl4``) away from the partition boundary.
