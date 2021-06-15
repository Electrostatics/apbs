.. _press:

press
=====

.. currentmodule:: apbs.input_file.calculate.nonpolar

.. note::  

   This command has been ported to the *new APBS syntax* (see :ref:`new_input_format`):  see :func:`Nonpolar.pressue` for more information.

This term specifies the solvent pressure in kJ mol\ :superscript:`-1` Å\ :superscript:`-3`.
This coefficient multiplies the volume term of the apolar model and can be set to zero to eliminate volume contributions to the apolar solvation calculation.
The syntax is:

.. code-block:: bash

   press {value}

where ``value`` is the floating point value of the pressure coefficient in kJ mol\ :superscript:`-1` Å\ :superscript:`-3`.

.. todo::

   Resolve unit confusion with geometric flow ``press`` keyword and the apolar :ref:`press` keyword.
   Documented in https://github.com/Electrostatics/apbs/issues/499
