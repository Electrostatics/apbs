.. _domainLength:

domainLength
============

.. todo::  This command has not yet been ported to the *new APBS syntax* (see :ref:`new_input_format`).

Specify the rectangular finite element mesh domain lengths for :ref:`femanual` finite element calculations.
This length may be different in each direction.
If the :ref:`usemesh` keyword is included, then this command is ignored.
The syntax is:

.. code-block:: bash

   domainLength {xlen ylen zlen}

where the parameters ``xlen ylen zlen`` are floating point numbers that specify the mesh lengths in the x-, y-, and z-directions (respectively) in units of Å.

