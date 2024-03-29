.. _chgm:

chgm
====

.. currentmodule:: apbs.input_file.calculate

.. note::  

   This command has been ported to the *new APBS syntax* (see :ref:`new_input_format`):
   
   * For finite difference, see :func:`finite_difference.FiniteDifference.charge_discretization` for more information.
   * For finite element, see :func:`finite_element.FiniteElement.charge_discretization` for more information.

Specify the method by which the biomolecular point charges (i.e., Dirac delta functions) by which charges are mapped to the grid for a multigrid (:ref:`mgmanual`, :ref:`mgauto`, :ref:`mgpara`) Poisson-Boltzmann calculation.
As we are attempting to model delta functions, the support (domain) of these discretized charge distributions is always strongly dependent on the grid spacing.
The syntax is:

.. code-block:: bash

   chgm {flag}

``flag`` is a text string that specifies the type of discretization:

``spl0``
  Traditional trilinear interpolation (linear splines).
  The charge is mapped onto the nearest-neighbor grid points.
  Resulting potentials are very sensitive to grid spacing, length, and position.
``spl2``
  Cubic B-spline discretization.
  The charge is mapped onto the nearest- and next-nearest-neighbor grid points.
  Resulting potentials are somewhat less sensitive (than ``spl0``) to grid spacing, length, and position.
``spl4``
  Quintic B-spline discretization.
  Similar to ``spl2``, except the charge/multipole is additionally mapped to include next-next-nearest neighbors (125 grid points receive charge density).

