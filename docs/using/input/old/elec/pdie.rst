.. _pdie:

pdie
====

Specify the dielectric constant of the solute molecule.
The syntax is:

.. code-block:: bash

   pdie {diel}

where ``diel`` is the floating point value of the unitless biomolecular dielectric constant.
This is usually a value between 2 to 20, where lower values consider only electronic polarization and higher values consider additional polarization due to intramolecular motion.
The dielectric value must be :math:`\ge 1`.
