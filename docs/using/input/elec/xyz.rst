.. _xyz:

xyz
===

For each molecule in the system and for each trajectory, specify a xyz file for the starting position of that molecule.
The syntax is:

.. code-block:: bash
   
   xyz {molecule_id} {filename}

``molecule_id``
  An integer (starting at 1) of the molecule index from the READ  section

``filename``
  The name of the file for the xyz coordinates of the molecule center for a given trajectory.
  The trajectories for a given molecule should be ordered sequentially in the ELEC section. </p>

.. todo::
   
   It would be nice to incorporate the ``xyz`` functionality into the :ref:`read` block.
   Documented in https://github.com/Electrostatics/apbs-pdb2pqr/issues/505
