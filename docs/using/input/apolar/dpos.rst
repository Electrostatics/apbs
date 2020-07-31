.. _dpos:

dpos
====

This is the displacement used for finite-difference-based calculations of surface area derivatives.
I know, this is a terrible way to calculate surface area derivatives -- we're working on replacing it with an analytic version.
In the meantime, please use this parameter with caution.
If anyone has code for a better method, please share!

The syntax is:

.. code-block:: bash

   dpos {displacement}

where ``displacement`` is a floating point number indicating the finite difference displacement for force (surface area derivative) calculations in units of Å.

.. warning::
   This parameter is very dependent on ``sdens`` (see :doc:`../generic/sdens`); e.g., smaller values of ``dpos`` require larger values of ``sdens``.
