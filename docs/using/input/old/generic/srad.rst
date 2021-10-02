.. _srad:

srad
====

.. note::  

   .. currentmodule:: apbs.input_file.calculate

   Some versions of this command have been ported to the *new APBS syntax* (see :ref:`new_input_format`):

   * Nonpolar calculations:  see :func:`nonpolar.Nonpolar.solvent_radius` for more information.
   * Finite-difference Poisson-Boltzmann calculations:  see :func:`finite_difference.FiniteDifference.solvent_radius` for more information.
   * Finite-element Poisson-Boltzmann calculations:  see :func:`finite_element.FiniteElement.solvent_radius` for more information.

This keyword specifies the radius of the solvent molecules; this parameter is used to define various solvent-related surfaces and volumes (see :ref:`elecsrfm`).
This value is usually set to 1.4 Å for a water-like molecular surface and set to 0 Å for a van der Waals surface.
The syntax is:

.. code-block:: bash

   srad {radius}

where ``radius`` is the floating point value of the solvent radius (in Å).
This keyword is ignored for ``srfm spl2`` (see :ref:`elecsrfm`).

