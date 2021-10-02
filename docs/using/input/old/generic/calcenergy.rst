.. _calcenergy:

calcenergy
==========

.. currentmodule:: apbs.input_file.calculate

.. note::  

   Some versions of this command have been ported to the *new APBS syntax* (see :ref:`new_input_format`):


   * Nonpolar calculations:  see :func:`nonpolar.Nonpolar.calculate_energy` for more information.
   * Polar calculations:

     * Finite difference:  see :func:`finite_difference.FiniteDifference.calculate_energy`
     * Finite element:  see :func:`finite_element.FiniteElement.calculate_energy`

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
