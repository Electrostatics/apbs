.. _fglen:

fglen
=====


.. currentmodule:: apbs.input_file.calculate.finite_difference

.. note::  

   This command has been ported to the *new APBS syntax* (see :ref:`new_input_format`):  see :func:`Focus.fine_grid_dimensions` for more information.


Specifies the fine mesh domain lengths in a multigrid focusing calculation (:ref:`mgpara` or :ref:`mgauto`); this may be different in each direction.
The syntax is:

.. code-block:: bash

   fglen {xlen ylen zlen}

This should enclose the region of interest in the molecule.
The arguments to this command are:

``xlen ylen zlen``
  Grid lengths (floating point numbers) in the x-, y-, and z-directions in Å.

