.. _femanual:

fe-manual
=========

Manually-configured adaptive finite element Poisson-Boltzmann calculations.

This is a single-point PBE calculation performed by our adaptive finite element PBE solver.
It requires that APBS be linked to the Michael Holst group `FEtk finite element library <http://www.fetk.org>`_ during compilation.
The finite element solver uses a "solve-estimate-refine" cycle.
Specifically, starting from an initial mesh, it performs the following iteration:

#. solve the problem with the current mesh
#. estimate the error in the solution
#. adaptively refine the mesh to reduce the error

This iteration is repeated until a global error tolerance is reached.

Keywords for this calculation type include:

.. toctree::
   :maxdepth: 2
   :caption: ELEC fe-manual keywords:

   akeyPRE
   akeySOLVE
   bcfl
   ../generic/calcenergy
   ../generic/calcforce
   chgm
   domainLength
   ekey
   etol
   ion
   lpbe
   lrpbe
   maxsolve
   maxvert
   ../generic/mol
   npbe
   nrpbe
   pdie
   ../generic/sdens
   sdie
   ../generic/srad
   srfm
   ../generic/swin
   targetNum
   targetRes
   ../generic/temp
   usemesh
   write


.. note::

   The finite element methods are currently most useful for a select set of problems which can benefit from adaptive refinement of the solution.
   Furthermore, this implementation is experimental.
   In general, the sequential and parallel focusing multigrid methods offer the most efficient solution of the PBE for most systems.
