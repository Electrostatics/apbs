.. _cglen:

cglen
=====

.. currentmodule:: apbs.input_file.calculate.finite_difference

.. note::  

   This command has been ported to the *new APBS syntax* (see :ref:`new_input_format`):  see :func:`Focus.coarse_grid_dimensions` for more information.

Specify the length of the coarse grid (in a focusing calculation) for an automatic multigrid (:ref:`mgauto`, :ref:`mgpara`) Poisson-Boltzmann calculation.
This may be different in each direction.

.. code-block:: bash

   cglen {xlen ylen zlen}

This is the starting mesh, so it should be large enough to complete enclose the biomolecule and ensure that the chosen boundary condition (see :ref:`bcfl`) is appropriate.

``xlen ylen zlen``
  Grid lengths (floating point numbers) in the x-, y-, and z-directions in Å.
