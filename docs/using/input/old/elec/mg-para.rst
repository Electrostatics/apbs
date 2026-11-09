.. _mgpara:

mg-para
=======

.. currentmodule:: apbs.input_file.calculate.finite_difference

.. note::

   This command has been ported to the *new APBS syntax* (see :ref:`new_input_format`); see :func:`Focus.parallel` and :func:`FiniteDifference.calculation_type`.


Automatically-configured parallel focusing multigrid Poisson-Boltzmann calculations.

This calculation closely resembles :ref:`mgauto` in syntax.
However, it is designed to perform electrostatics calculations on systems in a parallel focusing fashion.
When APBS is built without MPI and :ref:`async` is not set, ``mg-para`` runs all ``pdime`` partitions sequentially in one process and sums the finest-level energies and forces.
Use ``async`` to evaluate a single partition per invocation, or prefer :ref:`mgauto` with a larger ``dime`` on one node when the full grid fits in memory.

.. toctree::
   :maxdepth: 2
   :caption: ELEC mg-para keywords:

   async
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
   ofrac
   pdie
   pdime
   ../generic/sdens
   sdie
   ../generic/srad
   srfm
   ../generic/swin
   ../generic/temp
   usemap
   write
   writemat
