.. _tree_n0:

tree_n0
=======

.. currentmodule::  apbs.input_file.calculate.boundary_element

.. note::  This command has been ported to the *new APBS syntax* (see :ref:`new_input_format`); see :func:`TABIParameters.maximum_particles`.

TABI-PB parameter that specifies the maximum number of particles in a treecode leaf.
This controls leaf size in the process of building the tree structure.
The syntax is:

.. code-block:: bash

   tree_n0 {max_number}

where ``max_number`` is an integer.
A typical value for this parameter is 500.
