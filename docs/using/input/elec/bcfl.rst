.. _bcfl:

bcfl
====

Specifies the type of boundary conditions used to solve the Poisson-Boltzmann equation.
The syntax is:

.. code-block:: bash
   
   bcfl {flag}

where ``flag`` is a text string that identifies the type of conditions to be used.

``zero``
  "Zero" boundary condition. Dirichlet conditions where the potential at the boundary is set to zero.
  This condition is not commonly used and can result in large errors if used inappropriately.
``sdh``
  "Single Debye-Hückel" boundary condition.
  Dirichlet condition where the potential at the boundary is set to the values prescribed by a Debye-Hückel model for a single sphere with a point charge, dipole, and quadrupole.
  The sphere radius in this model is set to the radius of the biomolecule and the sphere charge, dipole, and quadrupole are set to the total moments of the protein.
  This condition works best when the boundary is sufficiently far from the biomolecule.
``mdh``
  "Multiple Debye-Hückel" boundary condition.
  Dirichlet condition where the potential at the boundary is set to the values prescribed by a Debye-Hückel model for a multiple, non-interacting spheres with a point charges.
  The radii of the non-interacting spheres are set to the atomic radii of and the sphere charges are set to the atomic charges.
  This condition works better than sdh for closer boundaries but can be very slow for large biomolecules.<br />
``focus``
  "Focusing" boundary condition.
  Dirichlet condition where the potential at the boundary is set to the values computed by the previous (usually lower-resolution) PB calculation.
  This is **only** used in sequential focusing performed manually in :ref:`mgmanual` calculations.
  All of the boundary points should lie within the domain of the previous calculation for best accuracy; if any boundary points lie outside, their values are computed using single Debye-Hückel boundary conditions (see above).
``map``
  Specifying map allows a previously calculated potential map to be used in a new focusing calculation.
  A typical scenario is using the same coarse grid for multiple focusing calculations.
  A potential map can be written once from a coarse grid calculation, then used in subsequent runs to bypass the need to recalculate the coarse grid.
  See the READ keyword pot (see :ref:`read`) and the attached example files for its use.


