.. _pbsamauto:

pbsam-auto
==========

PB-SAM is a semi-analytical solution to the linearized Poisson-Boltzmann equation for multiple molecules of arbitrary charge distribution in an ionic solution.
The solution is an extension of the :ref:`analytical method <pbamauto>`, leveraging fast-multipole methods as well as boundary elements.
Each molecule is coarse-grained as a system of overlapping spheres, whose surface charges are represented by multipole expansions.
For details on the method, please see `Yap, Head-Gordon (2010) <http://pubs.acs.org/doi/abs/10.1021/ct100145f>`_ and `Yap, Head-Gordon (2013) <http://pubs.acs.org/doi/abs/10.1021/ct400048q>`_.

.. todo::

   If there's only one mode to PBAM, let's call it ``pbsam`` instead of ``pbsam-auto``.

The current implementation of PB-SAM in APBS includes:

* Calculation of energies, forces and torques
* Calculation of electrostatic potentials
* Brownian dynamics simulations

Keywords for this calculation type include:

.. toctree::
   :maxdepth: 2
   :caption: ELEC pbsam-auto keywords:

   3dmap
   diff
   dx
   exp
   grid2d
   imat
   msms
   ntraj
   pbc
   pdie
   randorient
   runname
   runtype
   salt
   sdie
   surf
   ../generic/temp
   term
   termcombine
   tolsp
   units
   xyz

======================
Background information
======================

PB-SAM is a semi-analytical solution to the linearized Poisson-Boltzmann equation for multiple molecules of arbitrary charge distribution in an ionic solution.
The solution is an extension of the analytical method, leveraging Fast-Multipole methods as well as boundary elements.
Each molecule is coarse-grained as a system of overlapping spheres, whose surface charges are represented by the multipole expansions :math:`H^{(i)}` and :math:`F^{(i)}`.
To solve for the potential, the following interactions are considered:

* Intra-molecular interactions between overlapping spheres are treated numerically
* Intra-molecular interactions between non-overlapping spheres are treated analytically
* Inter-molecular interactions between spheres on different molecules

With these interactions, the multipole expansions are solved with an iterative SCF method, briefly given as

.. math::

   H^{(i,k)} &= I_{E}^{(i,k)} \cdot \left ( H^{(i,k)} + F^{(i,k)} + T \cdot H^{(j,l)} \right ) \\
   F^{(i,k)} &= I_{E}^{(i,k)} \cdot \left ( H^{(i,k)} + F^{(i,k)} + T \cdot F^{(j,l)} \right )

Where :math:`H^{(i)}` and :math`F^{(i)}` are multipole expansions, :math:`I_{E}^{(i,k)}` is the exposed surface integral matrix for sphere :math:`k` of molecule :math:`i`, and :math:`T` is an operator that transforms the multipole expansion to a local coordinate frame.

From the above formulation, computation of the interaction energy :math:`\Omega^{(i)}` for molecule :math:`i`, is given as a sum of all the interactions of spheres :math:`k` within it with all external spheres (in a simplified form) as follows:

.. math::

   \Omega^{(i)} = \frac{1}{\epsilon_s} \left \langle \sum_{k \, in\, i} \sum_{j \ne i}^N \sum_{l\, in \, j}  T \cdot H^{(j,l)} ,  H^{(i,k)} \right \rangle

where :math:`\langle  M, N \rangle` denotes the inner product.

When energy is computed, forces follow as:

.. math::

   \textbf{F}^{(i)} = \nabla_i \Omega^{(i)}=\frac{1}{\epsilon_s} [ \langle \nabla_i \,T \cdot H^{(j,l)} ,  H^{(i,k)} \rangle +  \langle T \cdot H^{(j,l)},   \nabla_i \, H^{(i,k)} \rangle

The method to calculate the torque is discussed in `Yap, Head-Gordon (2013) <http://pubs.acs.org/doi/abs/10.1021/ct400048q>`_.

============
PB-SAM files
============

-------------------
Vertex/surface file
-------------------

As part of the coarse-graining process a definition of the molecular surface is necessary.
For this we have historically used the program `MSMS <http://mgltools.scripps.edu/packages/MSMS>`_  or on the `online MSMS web server` <http://mgl.scripps.edu/people/sanner/html/msms_server.html>`_.
Within APBS, the user can implement the :doc:`msms` flag and the program will be run through APBS, but the executable must be included in your path.

If using the command-line MSMS tool, after downloading it for the correct platform, it can be run as follows:

.. code-block:: bash
   
   ./msms.system -if {filename}.xyzr -of {outfile}


It requires an :file:`{filename}.xyzr` file as input, which is the xyz coordinates of each atom of the system followed by the VdW radius.
This information can all be found in the PQR file.
MSMS will produce :file:`{outfile}.face` and :file:`{outfile}.vert` file.  
The vertex file is used to coarse-grain the molecule.
Once this has been generated, it can be used again as input using the :doc:`surf` command.

-----------------------
Coarse-grained PQR file
-----------------------

The coarse-graining process will produce a new PQR file :file:`mol{#}_cg.pqr` that contains the original PQR concatenated with coarse-graining spherical centers.
The number `#` refers to the order the file was read during the :doc:`../read` statements.

---------------------------
IMAT: surface integral file
---------------------------

The surface integrals are computed for the boundary element part of PB-SAM.
Their calculation can be quite time-consuming, so the first time they are computed for a system, they are saved to the working directory with the name :file:`mol{m}sph{s}.bin``.
The *m* in :file:`mol{m}sph{s}.bin`` is the ordered ID of the molecule from the PQR section.
The *s* in :file:`mol{m}sph{s}.bin`` refers to coarse-grained sphere *s* of the molecule.

-------------------------
Multipole expansion files
-------------------------

Much like the IMAT files, the expansion files are files generated from self-polarization that are useful and time-saving methods for running a system of full-mutual polarziation on many molecules.
If no expansion path is provided, the program will calculate self-polarization for each type of molecule in the system and save files of the form :file:`mol{m}{H,F}.{s}.exp`, where *m* is the molecule ID, *H* and *F* refer to the respective expansion (see above), and *s* is the coarse-grained sphere number.
