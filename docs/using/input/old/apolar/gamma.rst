.. _gamma:

gamma
=====

.. currentmodule:: apbs.input_file.calculate.nonpolar

.. note::  

   This command has been ported to the *new APBS syntax* (see :ref:`new_input_format`):  see :func:`Nonpolar.surface_tension` for more information.

This keyword specifies the surface tension coefficient for apolar solvation models.

.. code-block:: bash

   gamma { value }

where ``value`` is a floating point number designating the surface tension in units of kJ mol\ :superscript:`-1` Å\ :superscript:`-2`.
This term can be set to zero to eliminate the :abbr:`SASA (solvent-accessible surface area)` contributions to the apolar solvation calculations.

.. todo::

   Resolve unit confusion with geometric flow :ref:`gamma` keyword.
   https://github.com/Electrostatics/apbs/issues/490