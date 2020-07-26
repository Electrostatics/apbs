.. _pbamauto:

pbam-auto
=========

PB-AM is an analytical solution to the linearized Poisson-Boltzmann equation for multiple spherical objects of arbitrary charge distribution in an ionic solution.
More details on the method are available in `Lotan, Head-Gordon (2006) <http://pubs.acs.org/doi/full/10.1021/ct050263p>`_.
The physical calculations are uses to perform various actions on a system of molecules such as calculation of energies, forces, torques, electrostatic potentials, and Brownian dynamics schemes.
This fast method coarse-grains all molecules of the system into single spheres large enough to contain all molecule atoms.

.. todo::

   If there's only one mode to PBAM, let's call it ``pbam`` instead of ``pbam-auto``.
   Documented in https://github.com/Electrostatics/apbs-pdb2pqr/issues/498

The current implementation of PB-AM in APBS includes:

* Calculation of energies, forces and torques
* Calculation of electrostatic potentials
* Brownian dynamics simulations

Keywords for this calculation type include:

.. toctree::
   :maxdepth: 2
   :caption: ELEC pbam-auto keywords:

   3dmap
   diff
   dx
   grid2d
   gridpts
   ../generic/mol
   ntraj
   pbc
   pdie
   randorient
   runname
   runtype
   salt
   sdie
   ../generic/temp
   term
   termcombine
   units
   xyz

======================
Background information
======================

PB-AM is an analytical solution to the linearized Poisson-Boltzmann equation for multiple spherical objects of arbitrary charge distribution in an ionic solution.
The solution can be reduced to a simple system of equations as follows:

.. math::

   A = \Gamma \cdot (\Delta \cdot T \cdot A + E) 

Where :math:`A^{(i)}` represents the effective multipole expansion of the charge distributions of molecule :math:`i`.
:math:`E^{(i)}` is the free charge distribution of molecule :math:`i`.
:math:`\Gamma` is a dielectric boundary-crossing operator, :math:`\Delta` is a cavity polarization operator, :math:`T` an operator that transforms the multipole expansion to a local coordinate frame.
:math:`A^{(i)}` is solved for through an iterative SCF method.


From the above formulation, computation of the interaction energy :math:`\Omega^{(i)}` for molecule :math:`i`, is given as follows:

.. math::

   \Omega^{(i)}=\frac{1}{\epsilon_s} \left \langle \sum_{j \ne i}^N  T \cdot A^{(j)} ,  A^{(i)} \right \rangle

where :math:`\langle M, N \rangle` denotes the inner product.
Forces can be obtained from

.. math::

   \textbf{F}^{(i)} = \nabla_i \Omega^{(i)}=\frac{1}{\epsilon_s} \left[ \langle \nabla_i \,T \cdot A^{(i)} ,  A^{(i)} \rangle +  \langle T \cdot A^{(i)} ,   \nabla_i \, A^{(i)} \rangle \right]

