.. _mgmanual:

mg-manual
=========


Manually-configured finite differnece multigrid Poisson-Boltzmann calculations.

This is a standard single-point multigrid PBE calculation without focusing or additional refinement.
The ``mg-manual`` calculation offers the most control of parameters to the user.
Several of these calculations can be strung together to perform focusing calculations by judicious choice of the :ref:`bcfl` flag; however, the setup of the focusing is not automated as it is in :ref:`mgauto` and :ref:`mgpara` calculations and therefore this command should primarily be used by more experienced users.

.. toctree::
   :maxdepth: 2
   :caption: ELEC mg-manual keywords:

   bcfl
   ../generic/calcenergy
   ../generic/calcforce
   chgm
   dime
   etol
   gcent
   glen
   ../generic/grid
   ion
   lpbe
   lrpbe
   ../generic/mol
   nlev
   npbe
   pdie
   ../generic/sdens
   sdie
   ../generic/srad
   srfm
   ../generic/swin
   ../generic/temp
   usemap
   write
   writemat
