.. _calcforce:

calcforce
=========

This optional keyword controls energy output from an apolar solvation calculation.
The syntax is:

.. code-block:: bash

   calcforce {flag}

where ``flag`` is a text string that specifies the types of force values to be returned:

``no``
  (Deprecated) don't calculate any forces.
``total``
  Calculate and return total electrostatic and apolar forces for the entire molecule.
``comps``
  Calculate and return total electrostatic and apolar forces for the entire molecule as well as force components for each atom.<br />

The possible outputs from calcforce are:

``tot {n}``
  total force for atom *n*
``qf {n}``
  fixed charge force for atom *n*
``db {n}``
  dielectric boundary force for atom *n*
``ib {n}``
  ionic boundary force for atom *n*

The values will be printed in three columns which correspond to the x, y, and z components of the force vector.

.. note::
   This option must be used consistently (with the same ``flag`` value) for all calculations that will appear in subsequent :ref:`print` statements.
