.. _pbc:

pbc
===

This keyword is used to indicate if 3D periodic boundary conditions (PBCs) will be used in a PB-(S)AM calculation.
If used, a box length must also be specified, in Ångstroms.

.. code-block:: bash
   
   pbc {boxlength}

where ``boxlength`` is the floating point value of the box length in Ångstroms.

.. note::

   The box is centered at the origin (0, 0, 0).
   The code assumes a minimum image convention, so it only includes the closest image of the neighboring molecules.
   For this convention to always be preserved, the periodic box is assumed to be large enough such that the electrostatic forces are sufficiently attenuated beyond one boxlength.
   Generally, the program assumes a mutual polarization cutoff of 100 Å for the mutual polarization, so if the boxlength is shorter, the cutoff will be reduced to boxlength/2.
