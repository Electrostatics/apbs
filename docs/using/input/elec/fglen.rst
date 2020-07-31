.. _fglen:

fglen
=====

Specifies the fine mesh domain lengths in a multigrid focusing calculation (:ref:`mgpara` or :ref:`mgauto`); this may be different in each direction.
The syntax is:

.. code-block:: bash

   fglen {xlen ylen zlen}

This should enclose the region of interest in the molecule.
The arguments to this command are:

``xlen ylen zlen``
  Grid lengths (floating point numbers) in the x-, y-, and z-directions in Å.

