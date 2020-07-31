.. _bconc:

bconc
=====

This keyword specifies the bulk solvent density.
This coefficient multiplies the integral term of the apolar model discussed above and can be set to zero to eliminate integral contributions to the apolar solvation calculation.
The syntax is:

.. code-block:: bash

   bconc <density>

where ``density`` is a floating point number giving the bulk solvent density in Å\ :superscript:`-3`.
