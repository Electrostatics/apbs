.. _diff:

diff
====

Specify the diffusion coefficients for each molecule in the system for a PB-(S)AM Brownian dynamics calculation.

.. code-block:: bash

   diff {type} {dTrans} {dRot}

``type``
  a string indicating the molecule dynamics type
  
  ``stat``
    Stationary.

  ``rot``
    Object is fixed but rotates

  ``move``
    Object moves and rotates.

``dTrans``
  Translational diffusion coefficient in units of Å\ :sup:`2` ps\ :sup:`-1`.
  Used only with the ``move`` keyword.

``dRot``
  Rotational diffusion coefficient.
  Used with the ``move`` and ``rot`` keywords.

.. todo::
   
   What are the units for ``dRot``?
   Documented as https://github.com/Electrostatics/apbs-pdb2pqr/issues/486

.. note::

   The order of these keywords is expected to be identical to the order of the molecules in the READ section.

.. todo::
   
   Add a ``mol id`` flag rather than have an implicit ordering of the ``diff`` keywords.
   Documented in https://github.com/Electrostatics/apbs-pdb2pqr/issues/487
