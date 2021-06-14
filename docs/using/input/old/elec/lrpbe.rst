.. _lrpbe:

lrpbe
=====

Specifies that the linear form of the regularized Poisson-Boltzmann equation (RPBE) should be solved.
The regularized PBE equation replaces the point charge distribution with the corresponding Green's function.
As a result of this replacement, the solution corresponds to the reaction field instead of the total potential; the total potential can be recovered by adding the appropriate Coulombic terms to the solution.
Likewise, this equation immediately yields the solvation energy without the need for reference calculations.

.. note::

   The options :ref:`lpbe`, :ref:`npbe`, :ref:`lrpbe`, :ref:`nrpbe` are mutually exclusive.