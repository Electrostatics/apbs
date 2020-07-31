.. _runtype:

runtype
=======

Indicate what type of calculation you would like to run with the PB-(S)AM model.

.. code-block:: bash
   
   runtype {type}

where ``type`` is the type of calculation to be perfomed:

``energyforce``
  Compute and print out the interaction energies, forces and torques on each molecule. 

``electrostatics``
  Print the electrostatic potential of points in the system.

``dynamics``
  Perform a Brownian Dynamics simulation, using forces and torques generated from the PB-(S)AM model.
  The calculation of force and torque has been integrated into a Brownian dynamics scheme that is detailed in `Yap EH, Head-Gordon TL (2013) <http://pubs.acs.org/doi/abs/10.1021/ct400048q>`_
  This option will generate a series of files of the form

  :file:`dyn_{toy}.pqr`
    The starting configuration of the system for the first trajectory

  :file:`dyn_{toy}.stat`
    A file that prints how each trajectory was terminated and the time that this occurred at.

  :file:`dyn_{toy}_traj.xyz`
    A VMD-readable xyz file for the trajectory of ``traj``.

  :file:`dyn_toy_traj.dat`
    A file with positions, forces and torques for the system.

  .. todo::

     The dynamics part of the PB-(S)AM code should be moved out of the ``ELEC`` section.
     Documented in https://github.com/Electrostatics/apbs-pdb2pqr/issues/500
