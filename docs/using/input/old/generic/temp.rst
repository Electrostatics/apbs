.. _temp:

temp
====

.. note::  

   Some versions of this command have been ported to the *new APBS syntax* (see :ref:`new_input_format`):


   * Nonpolar calculations:
      .. currentmodule:: apbs.input_file.calculate.nonpolar

      See :func:`Nonpolar.temperature` for more information.

This keyword specifies the temperature for the calculation.
The syntax is:

.. code-block:: bash

   temp {T}

where ``T`` is the floating point value of the temperature for calculation (in K).
