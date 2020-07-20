.. _calcenergy:

calcenergy
==========

This optional keyword controls energy output from an apolar solvation calculation.
The syntax is:

.. code-block:: bash

   calcenergy <flag>

where ``flag`` is a string denoting what type of energy to calculate:

``no``
  (Deprecated) Don't calculate any energies.
``total``
  Calculate and return total apolar energy for the entire molecule.
``comps``
  Calculate and return total apolar energy for the entire molecule as well as the energy components for each atom.

.. note::
   This option must be used consistently (with the same ``flag`` value) for all calculations that will appear in subsequent :ref:`print` statements.
