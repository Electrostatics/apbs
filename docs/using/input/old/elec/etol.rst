.. _etol:

etol
====

.. currentmodule:: apbs.input_file.calculate

.. note::  

   Some versions of this command have been ported to the *new APBS syntax* (see :ref:`new_input_format`):

   * For a finite difference calculation, see :func:`finite_difference.FiniteDifference.error_tolerance`.
   * For a finite element calculation, see :func:`finite_element.FiniteElement.error_tolerance`.

.. todo::  add documentation links for other instances.

Specifies the tolerance for iterations of the partial differntial equation solvers:
The syntax is:

.. code-block:: bash
   
   etol { tol }

where ``tol`` is the (floating point) numerical value for the error tolerance.

For finite difference solvers, this keyword is optional and is intended for :ref:`mgmanual`, :ref:`mgauto`, and :ref:`mgpara` calculation types.

For finite element solvers, this keyword specifies the tolerance for error-based adaptive refinement during the solve-estimate-refine iterations of the finite element solver (:ref:`femanual`), where ``tol`` is the (floating point) numerical value for the error tolerance.
