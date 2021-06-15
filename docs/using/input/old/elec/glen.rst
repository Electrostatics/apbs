.. _glen:

glen
====

.. todo::  This command has not yet been ported to the *new APBS syntax* (see :ref:`new_input_format`).

Specify the mesh domain lengths for multigrid :ref:`mgmanual` calculations.
These lengths may be different in each direction.
The syntax is:

.. code-block:: bash
   
   glen {xlen ylen zlen}

where ```xlen ylen zlen`` are the (floating point) grid lengths in the x-, y-, and z-directions (respectively) in Å.
