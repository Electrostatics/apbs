.. _cgcent:

cgcent
======

.. currentmodule:: apbs.input_file.calculate.finite_difference

.. note::  

   This command has been ported to the *new APBS syntax* (see :ref:`new_input_format`):  see :func:`Focus.coarse_grid_center` for more information.

This keyword controls electrostatic energy output from a Poisson-Boltzmann calculation
The syntax is:

.. code-block:: bash

   cgcent { mol id | xcent ycent zcent }

The arguments for this keyword are **either**

``mol id``
  Center the grid on molecule with integer ID ``id``; as assigned in the ``READ`` section with a ``READ mol`` command (see :ref:`read_old_input`)

**or**

``xcent ycent zcent``
  Center the grid on the (floating point) coordinates (in Å) at which the grid is centered.
  Based on the PDB coordinate frame.

