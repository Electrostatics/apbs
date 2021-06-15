.. _units:

units
=====

.. todo::  This command has not yet been ported to the *new APBS syntax* (see :ref:`new_input_format`).

Specify the units for energy/force/potential output in PB-(S)AM calculations:

.. code-block:: bash
   
   units {flag}

where ``flag`` specifies the unit system:

``kcalmol``
  kcal/mol

``jmol``
  J/mol

``kT``
  kT

Force units will be energy units/Angstrom and potential units will be energy units/electron.

.. todo::

   It would be great to use the same units everywhere in APBS.
   Documented in https://github.com/Electrostatics/apbs/issues/485
