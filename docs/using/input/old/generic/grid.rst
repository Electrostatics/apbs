.. _grid:

grid
====

.. note::  

   Some versions of this command have been ported to the *new APBS syntax* (see :ref:`new_input_format`):


   * Nonpolar calculations:
      .. currentmodule:: apbs.input_file.calculate.nonpolar

      See :func:`Nonpolar.grid_spacings` for more information.

Specify the grid spacings for multigrid and volume integral calculations.
This value may be different in each direction.
The syntax is:

.. code-block:: bash

   grid {hx hy hz}

where ``hx hy hz`` are the (floating point) grid spacings in the x-, y-, and z-directions (respectively) in Å.
