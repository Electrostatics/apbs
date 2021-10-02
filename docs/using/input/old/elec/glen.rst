.. _glen:

glen
====

.. currentmodule:: apbs.input_file.calculate.finite_difference

.. note::  

   This command has been ported to the *new APBS syntax* (see :ref:`new_input_format`); see :class:`GridDimensions` for more information.

   
Specify the mesh domain lengths for multigrid :ref:`mgmanual` calculations.
These lengths may be different in each direction.
The syntax is:

.. code-block:: bash
   
   glen {xlen ylen zlen}

where ```xlen ylen zlen`` are the (floating point) grid lengths in the x-, y-, and z-directions (respectively) in Å.
