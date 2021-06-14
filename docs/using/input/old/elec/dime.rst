.. _dime:

dime
====

Specifies the number of grid points per processor for grid-based discretization.
The syntax is:

.. code-block:: bash
   
   dime {nx ny nz}

For :ref:`mgmanual` calculations, the arguments are dependent on the choice of :ref:`nlev` by the formula: :math:`n = c 2^{l + 1} + 1` where *n* is the dime argument, *c* is a non-zero integer, *l* is the :ref:`nlev` value.
The most common values for grid dimensions are 65, 97, 129, and 161 (they can be different in each direction); these are all compatible with a :ref:`nlev` value of 4.
If you happen to pick a "bad" value for the dimensions (i.e., mismatch with :ref:`nlev`), the APBS code will adjust the specified :ref:`dime` downwards to more appropriate values.
This means that "bad" values will typically result in lower resolution/accuracy calculations!
The arguments for this keyword are:

``nx ny nz``
  The (integer) number of grid points in the x-, y-, and z-directions, respectively.

.. note::
   dime should be interpreted as the number of grid points per processor for all calculations, including :ref:`mgpara`.
   This interpretation helps manage the amount of memory per-processor - generally the limiting resource for most calculations.

