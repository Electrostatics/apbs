.. _sdie:

sdie
====

Specify the dielectric constant of the solvent.
The syntax is:

.. code-block:: bash
   
   sdie {diel}

where ``diel`` is a floating point number representing the solvent dielectric constant (unitless).
This number must be :math:`\ge 1`.
Bulk water at biologically-relevant temperatures is usually modeled with a dielectric constant of 78-80.
