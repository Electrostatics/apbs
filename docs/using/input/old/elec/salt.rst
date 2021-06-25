.. _salt:

salt
====

.. todo::  This command has not yet been ported to the *new APBS syntax* (see :ref:`new_input_format`).

Specify the monovalent salt concentration of the system, in molar. This is usually a value between 0.00 to 0.15.

.. code-block:: bash
   
   salt {saltConc}

where ``saltConc`` is the floating point value of the monovalent salt concentration in molar.

.. todo::

   The PB-(S)AM ``salt`` keyword should be eradicated and replaced with the :ref:`ion` keyword.
   Documented in https://github.com/Electrostatics/apbs/issues/501
   
