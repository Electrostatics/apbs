.. _swin:

swin
====

.. currentmodule::  apbs.input_file.calculate

.. note::


   Some instances of this keyword have been moved to the *new APBS syntax* (see :ref:`new_input_format`):

   * For finite difference calculations, see :func:`finite_difference.FiniteDifference.surface_spline_window`
   * For finite element calculations, see :func:`finite_element.FiniteElement.surface_spline_window`

   .. todo::  move other instances of this keyword to the new syntax

Specify the size of the support (i.e., the rate of change) for spline-based surface definitions (see :ref:`elecsrfm`).
The syntax is:

.. code-block:: bash
   
   swin {win}

where ``win`` is a floating point number for the spline window width (in Å).
Usually 0.3 Å.

Note that, per the analysis of Nina, Im, and Roux (`article <http://dx.doi.org/10.1016/S0301-4622(98)00236-1>`_)</a>, the force field parameters (radii) generally need to be adjusted if the spline window is changed.
