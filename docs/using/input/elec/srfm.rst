.. _elecsrfm:

srfm (elec)
===========

Specify the model used to construct the dielectric and ion-accessibility coefficients.
The syntax for this command is:

.. code-block:: bash

   srfm {flag}

where ``flag`` is a string describing the coefficient model:

``mol``
  The dielectric coefficient is defined based on a molecular surface definition.
  The problem domain is divided into two spaces.
  The "free volume" space is defined by the union of solvent-sized spheres (see :ref:`srad`) which do not overlap with biomolecular atoms.
  This free volume is assigned bulk solvent dielectric values.
  The complement of this space is assigned biomolecular dielectric values.
  With a non-zero solvent radius (srad), this choice of coefficient corresponds to the traditional definition used for PB calculations.
  When the solvent radius is set to zero, this corresponds to a van der Waals surface definition.
  The ion-accessibility coefficient is defined by an "inflated" van der Waals model.
  Specifically, the radius of each biomolecular atom is increased by the radius of the ion species (as specified with the :ref:`ion` keyword).
  The problem domain is then divided into two spaces.
  The space inside the union of these inflated atomic spheres is assigned an ion-accessibility value of 0; the complement space is assigned bulk ion accessibility values.

``smol``
  The dielectric and ion-accessibility coefficients are defined as for mol (see above).
  However, they are then "smoothed" by a 9-point harmonic averaging to somewhat reduce sensitivity to the grid setup as described by Bruccoleri et al. J Comput Chem 18 268-276, 1997 (`10.1007/s00214-007-0397-0 <http://dx.doi.org/10.1007/s00214-007-0397-0>`_).

``spl2``
  The dielectric and ion-accessibility coefficients are defined by a cubic-spline surface as described by Im et al, Comp Phys Commun 111 (1-3) 59-75, 1998 (`10.1016/S0010-4655(98)00016-2 <https://doi.org/10.1016/S0010-4655(98)00016-2>`_).
  The width of the dielectric interface is controlled by the :ref:`swin` parameter.
  These spline-based surface definitions are very stable with respect to grid parameters and therefore ideal for calculating forces.
  However, they require substantial reparameterization of the force field; interested users should consult Nina et al, Biophys Chem 78 (1-2) 89-96, 1999 (`10.1016/S0301-4622(98)00236-1 <http://dx.doi.org/10.1016/S0301-4622(98)00236-1>`_).
  Additionally, these surfaces can generate unphysical results with non-zero ionic strengths; this is an on-going area of development.

``spl4``
  The dielectric and ion-accessibility coefficients are defined by a 7th order polynomial.
  This surface definition has characteristics similar to spl2, but provides higher order continuity necessary for stable force calculations with atomic multipole force fields (up to quadrupole).
