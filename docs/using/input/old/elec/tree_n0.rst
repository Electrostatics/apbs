.. _tree_n0:

tree_n0
=======

.. todo::  This command has not yet been ported to the *new APBS syntax* (see :ref:`new_input_format`).

TABI-PB parameter that specifies the maximum number of particles in a treecode leaf.
This controls leaf size in the process of building the tree structure.
The syntax is:

.. code-block:: bash
   
   tree_n0 {max_number}

where ``max_number`` is an integer.
A typical value for this parameter is 500.
