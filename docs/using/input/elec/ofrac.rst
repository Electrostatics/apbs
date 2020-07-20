.. _ofrac:

ofrac
=====

Specify the amount of overlap to include between the individual processors meshes in a parallel focusing calculation (:ref:`mgpara`).
The syntax is:

.. code-block:: bash

   ofrac {frac}

where ``frac`` is a floating point value between 0.0 and 1.0 denoting the amount of overlap between processors.
Empirical evidence suggests that an value of 0.1 is sufficient to generate stable energies.
However, this value may not be sufficient to generate stable forces and/or good quality isocontours.
For example, the following table illustrates the change in energies and visual artifacts in isocontours as a function of ofrac values for a small peptide (2PHK:B).

.. list-table:: Sensitivity of 2PHK:B solvation energy calculations to ofrac values.
   :widths: auto
   :header-rows: 1

   * - ``ofrac`` value
     - Energy (kJ/mol)
     - Visual artifact in isocontour?
   * - 0.05
     - 342.79
     - No
   * - 0.06
     - 342.00
     - No
   * - 0.07
     - 341.12
     - Yes
   * - 0.08
     - 341.14
     - Yes
   * - 0.09
     - 342.02
     - Yes
   * - 0.10
     - 340.84
     - Yes
   * - 0.11
     - 339.67
     - No
   * - 0.12
     - 341.10
     - No
   * - 0.13
     - 341.10
     - No
   * - 0.14
     - 341.32
     - No
   * - 0.15
     - 341.54
     - No

In general, larger <code>ofrac</code> values will reduce the parallel efficiency but will improve the accuracy.

For broad spatial support of the splines, every charge included in partition needs to be at least 1 grid space (:ref:`chgm` ``spl0``), 2 grid spaces (:ref:`chgm` ``spl2``), or 3 grid spaces (:ref:`chgm` ``spl4``) away from the partition boundary.

