.. _mgauto:

mg-auto
=======

Automatically configured finite difference Poisson-Boltzmann calculations.

This multigrid calculation automatically sets up and performs a string of single-point PBE calculations to "focus" on a region of interest (binding site, etc.) in a system.
It is basically an automated version of :ref:`mgmanual` designed for easier use.
Most users should use this version of ELEC.

Focusing is a method for solving the Poisson-Boltzmann equation in a finite difference setting.
Some of the earliest references to this method are from Gilson and Honig [#Gilson]_.
The method starts by solving the equation on a coarse grid (i.e., few grid points) with large dimensions (i.e., grid lengths).
The solution on this coarse grid is then used to set the Dirichlet boundary condition values for a smaller problem domain -- and therefore a finer grid -- surrounding the region of interest.
The finer grid spacing in the smaller problem domain often provides greater accuracy in the solution.

The following keywords are present in mg-auto ELEC blocks; all keywords are required unless otherwise noted.

.. note::

   During focusing calculations, you may encounter the message "WARNING! Unusually large potential values detected on the focusing boundary!" for some highly charged systems based on location of the focusing boundary.
   First, you should determine if you received any other warning or error messages as part of this calculation, particularly those referring to exceeded number of iterations or error tolerance (:ref:`etol`). 
   Next, you should check if the calculation converged to a reasonable answer.
   In particular, you should check sensitivity to the grid spacing by making small changes to the grid lengths (via the :ref:`fglen` parameter) and see if the changes in energies are correspondingly small.
   If so, then this warning can be safely ignored.


.. toctree::
   :maxdepth: 2
   :caption: ELEC mg-auto keywords:

   bcfl
   ../generic/calcenergy
   ../generic/calcforce
   cgcent
   cglen
   chgm
   dime
   etol
   fgcent
   fglen
   ion
   lpbe
   lrpbe
   ../generic/mol
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

.. [#Gilson] Gilson MK and Honig BH, Calculation of electrostatic potentials in an enzyme active site. Nature, 1987. 330(6143): p. 84-6. DOI:`10.1038/330084a0 <http://dx.doi.org/10.1038/330084a0>`_