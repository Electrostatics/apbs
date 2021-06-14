.. _apolar:

APOLAR input file section
=========================

This section is the main component for apolar solvation calculations in APBS runs.
There may be several APOLAR sections, operating on different molecules or using different parameters for multiple runs on the same molecule.
The syntax of this section is:

.. code-block:: bash

   APOLAR [name id]
     <keywords...>
   END

The first (optional) argument is:

.. code-block:: bash

   name <id>

where ``id`` is a unique string which can be assigned to the calculation to facilitate later operations (particularly in the :doc:`../print` statements).
The ``keywords...`` describing the parameters of the apolar calculation are discussed in more detail below:

.. toctree::
   :maxdepth: 2
   :caption: APOLAR keywords:

   ../generic/bconc
   ../generic/calcenergy
   ../generic/calcforce
   dpos
   gamma
   ../generic/grid
   ../generic/mol
   press
   ../generic/sdens
   ../generic/srad
   srfm
   ../generic/swin
   ../generic/temp

APBS apolar calculations follow the very generic framework described in  Wagoner JA, Baker NA. Assessing implicit models for nonpolar mean solvation forces: the importance of dispersion and volume terms. Proc Natl Acad Sci USA, 103, 8331-8336, 2006. doi:`10.1073/pnas.0600118103 <http://dx.doi.org/10.1073/pnas.0600118103>`_.
`
Nonpolar solvation potentials of mean force (energies) are calculated according to:

.. math::

   {W}^{(\mathrm{np})}(x) = \gamma A(x) + pV(x) + \bar \rho \sum^N_{i=1} \int _{\Omega} u_i^{(\mathrm{att})} (x_i, y) \theta (x,y) \, \mathrm{d}y 

and mean nonpolar solvation forces are calculated according to:

.. math::

   \mathbf{F}_i^{(\mathrm{np})}(x) = -\gamma \frac{\partial A (x)}{\partial x_i} - p \int _{\Gamma _i (x)} \frac{y-x_i}{\lVert y - x_i \rVert} \, \mathrm{d}y - \bar \rho \sum _{i=1}^N \int _{\Omega} \frac{\partial u_i^{(\mathrm{att})}(x_i,y)}{\partial x_i} \theta (x,y) \, \mathrm{d}y 

In these equations, :math:`\gamma` is the repulsive (hard sphere) solvent surface tension (see :ref:`gamma`), *A* is the conformation-dependent solute surface area (see :ref:`srad` and :ref:`apolarsrfm` keywords), *p* is the repulsive (hard sphere) solvent pressure (see :ref:`press` keyword), *V* is the conformation-dependent solute volume (see :ref:`srad` and :ref:`apolarsrfm` keywords), :math:`\rho` (see :ref:`bconc` keywords) is the bulk solvent density, and the integral involves the attractive portion (defined in a Weeks-Chandler-Andersen sense) of the Lennard-Jones interactions between the solute and the solvent integrated over the region of the problem domain outside the solute volume *V*.
Lennard-Jones parameters are taken from APBS parameter files as read in through an APBS input file READ statement (see :ref:`read`).

.. note::

   The above expressions can easily be reduced to simpler apolar solvation formalisms by setting one or more of the coefficients to zero through the keywords.

.. warning::

   All APOLAR calculations require a parameter file which contains Lennard-Jones radius and well-depth parameters for all the atoms in the solute PDB.
   This parameter file must also contain radius and well-depth parameters for water (specifically: residue "WAT" and atom "OW").
   Complete parameter files for protein and nucleic acid parameters are not currently available; we prefer geometric flow calculations (coupled polar and apolar components) rather than this model.
   