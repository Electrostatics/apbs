.. _temp:

temp
====

.. note::  

   Some versions of this command have been ported to the *new APBS syntax* (see :ref:`new_input_format`):

   .. currentmodule:: apbs.input_file.calculate

   * Finite difference calculations: See :func:`finite_difference.FiniteDifference.temperature`.
   * Nonpolar calculations: See :func:`nonpolar.Nonpolar.temperature`.

   .. todo:: add other uses to new syntax

This keyword specifies the temperature for the calculation.
The syntax is:

.. code-block:: bash

   temp {T}

where ``T`` is the floating point value of the temperature for calculation (in K).
