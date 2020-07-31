.. _units:

units
=====

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
   Documented in https://github.com/Electrostatics/apbs-pdb2pqr/issues/485
