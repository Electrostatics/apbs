.. _sdie:

sdie
====

.. note:: 

   Some versions of this command have been ported to the *new APBS syntax* (see :ref:`new_input_format`):

   .. currentmodule::  apbs.input_file.calculate.finite_difference

   * Finite difference Poisson-Boltzmann calculations: see :func:`FiniteDifference.solvent_dielectric` for more information.

.. todo:: port for other calculation types

Specify the dielectric constant of the solvent.
The syntax is:

.. code-block:: bash
   
   sdie {diel}

where ``diel`` is a floating point number representing the solvent dielectric constant (unitless).
This number must be :math:`\ge 1`.
Bulk water at biologically-relevant temperatures is usually modeled with a dielectric constant of 78-80.
