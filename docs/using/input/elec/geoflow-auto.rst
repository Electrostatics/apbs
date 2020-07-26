.. _geoflowauto:

geoflow-auto
============

To increase the accuracy of our implicit solvent modeling, we have implemented a differential geometry based geometric flow
solvation model `(Thomas, 2013) <https://www.ncbi.nlm.nih.gov/pubmed/23212974>`_.
In this model, polar and nonpolar solvation free energies are coupled and the solvent-solute boundary is determined in a self-consistent manner.
Relevant references are provided in :doc:`Recommended reading </reading>`.
This section provides a brief overview of the method.

The solutions for the electrostatic potential :math:`\phi` and the characteristic function :math:`S` (related to the solvent density) are obtained by minimizing a free energy functional that includes both polar and nonpolar solvation energy terms.
Minimization of the functional with respect to :math:`\phi` gives the Poisson-Boltzmann equation with a dielectric coefficient :math:`\epsilon` has the solute value :math:`\epsilon_m` where :math:`S = 1` and the solvent value :math:`\epsilon_s` where :math:`S = 0`.
Minimization of the free energy functional with respect to :math:`S` gives

.. math::

   -\nabla\cdot\left(\gamma\frac{\nabla S}{\parallel\nabla S\parallel}\right)+p-\rho_0U^{att}+\rho_m\phi - \frac{1}{2}\epsilon_m\mid\nabla\phi\mid^2+\frac{1}{2}\epsilon_s\mid\nabla\phi\mid^2=0 

where :math:`\gamma` is the microscopic surface tension, :math:`p` is the hydrostatic pressure, and :math:`U^{att}` is the attractive portion of the van der Waals dispersion interaction between the solute and the solvent.

Keywords for this calculation type include:

.. toctree::
   :maxdepth: 2
   :caption: ELEC geoflow-auto keywords:

   bcfl
   ../generic/bconc
   etol
   gamma-geoflow
   lpbe
   ../generic/mol
   pdie
   press-geoflow
   sdie
   vdwdisp

.. warning::

   Although the ``ion`` and ``lpbe`` keywords will be accepted in the geoflow-auto calculation, the treatment of salt is not currently implemented in APBS geometric flow.

.. todo::
   
   Add LPBE/NPBE support to geometric flow or remove the ``ion`` and ``lpbe`` keywords.
   Documented in https://github.com/Electrostatics/apbs-pdb2pqr/issues/491

.. todo::
   
   If there's only one mode, then we can change the keyword from ``geoflow-auto`` to just ``geoflow``.
   Documented in https://github.com/Electrostatics/apbs-pdb2pqr/issues/492

