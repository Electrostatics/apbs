.. _grid2d:

grid2d
======

.. todo::  This command has not yet been ported to the *new APBS syntax* (see :ref:`new_input_format`).

Specify the filename and location of a 2D cross sectional potential to be written to.

.. code-block:: bash

   grid2d {filename} {axis} {axis_value}

``filename``
  String for the name of the 2D grid to be printed out

``axis``
  String of either x, y, or z, for which cartesian axis the grid will be computed along

``axis_value``
  A floating point number of the position along ``axis`` that will be used.

.. note::

   Multiple 2D files can be printed out with 1 PB-AM run. Just specify them with more grid2d flags.

.. todo::
   
   The PB-(S)AM ``grid2d`` keyword should not exist; please replace it ASAP with the :ref:`write` command.
   Documented in https://github.com/Electrostatics/apbs/issues/493

