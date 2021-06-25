.. _pdie:

pdie
====

.. note:: 

   Some versions of this command have been ported to the *new APBS syntax* (see :ref:`new_input_format`):

   .. currentmodule::  apbs.input_file.calculate.finite_difference

   * Finite difference Poisson-Boltzmann calculations: see :func:`FiniteDifference.solute_dielectric` for more information.

.. todo:: port for other calculation types

Specify the dielectric constant of the solute molecule.
The syntax is:

.. code-block:: bash

   pdie {diel}

where ``diel`` is the floating point value of the unitless biomolecular dielectric constant.
This is usually a value between 2 to 20, where lower values consider only electronic polarization and higher values consider additional polarization due to intramolecular motion.
The dielectric value must be :math:`\ge 1`.
