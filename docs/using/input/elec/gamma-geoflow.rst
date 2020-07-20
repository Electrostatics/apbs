gamma
=====

This keyword specifies the surface tension coefficient for apolar solvation models.

.. code-block:: bash

   gamma { value }

where ``value`` is a floating point number designating the surface tension in units of kcal mol\ :superscript:`-1` Å\ :superscript:`-2`.
This term can be set to zero to eliminate the :abbr:`SASA (solvent-accessible surface area)` contributions to the apolar solvation calculations.

.. warning::

   *Either* this documentation is incorrect *or* the implementation needs to be changed to use kJ mol\ :superscript:`-1` Å\ :superscript:`-2` instead of kcal.

.. todo::

   Resolve unit confusion with geometric flow :ref:`gamma` keyword.
   https://github.com/Electrostatics/apbs-pdb2pqr/issues/490