press
=====

This term specifies the solvent pressure in kJ mol\ :superscript:`-1` Å\ :superscript:`-3`.
This coefficient multiplies the volume term of the apolar model and can be set to zero to eliminate volume contributions to the apolar solvation calculation.
The syntax is:

.. code-block:: bash

   press {value}

where ``value`` is the floating point value of the pressure coefficient in kJ mol\ :superscript:`-1` Å\ :superscript:`-3`.

.. warning::

   *Either* this documentation is incorrect *or* the implementation needs to be changed to use kJ mol\ :superscript:`-1` Å\ :superscript:`-3` instead of kcal.

.. todo::

   Resolve unit confusion with geometric flow ``press`` keyword and the apolar :ref:`press` keyword.
   Documented in https://github.com/Electrostatics/apbs-pdb2pqr/issues/499
