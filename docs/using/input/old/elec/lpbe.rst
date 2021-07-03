.. _lpbe:

lpbe
====

.. currentmodule:: apbs.input_file.calculate

.. note::  

   Some aspects of this command have been moved to the *new APBS syntax* (see :ref:`new_input_format`): 

   * Finite difference:  see :func:`finite_difference.FiniteDifference.equation` for more information.

   * Finite element:  see :func:`finite_element.FiniteElement.equation` for more information.

.. todo::  port for other types of calculations.


Specifies that the linearized Poisson-Boltzmann equation should be solved.

.. note::

   The options :ref:`lpbe`, :ref:`npbe`, :ref:`lrpbe`, :ref:`nrpbe` are mutually exclusive.