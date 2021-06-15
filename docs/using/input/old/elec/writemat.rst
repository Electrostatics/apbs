.. _writemat:

writemat
========

.. todo::  This command has not yet been ported to the *new APBS syntax* (see :ref:`new_input_format`).

This controls the output of the mathematical operators in the Poisson-Boltzmann equation as matrices in Harwell-Boeing matrix format (multigrid only).
The syntax is:

.. code-block:: bash
   
   writemat {type} {stem}

where

``type``
  A string that indicates what type of operator to output.

  ``poisson``
    Write out the Poisson operator :math:`-\nabla \cdot \epsilon \nabla`.

``stem``
  A string that specifies the path for the output.
