.. _usemap:

usemap
======

Specify pre-calculated coefficient maps to be used in the Poisson-Boltzmann calculation.
These must have been input via an earlier READ statement (see :ref:`read`).

The syntax for this command is:

.. code-block:: bash
   
   usemap {type} {id}

where the mandatory keywords are:

``type``
  A string that specifies the type of pre-calculated map to be read in:

  ``diel``
    Dielectric function map (as read by :ref:`read` ``diel``); this causes the :ref:`pdie`, :ref:`sdie`, :ref:`srad`, :ref:`swin`, and :ref:`elecsrfm` parameters and the radii of the biomolecular atoms to be ignored when computing dielectric maps for the Poisson-Boltzmann equation.
    Note that the :ref:`pdie` and :ref:`sdie` values are still used for some boundary condition calculations as specified by :ref:`bcfl`.
  ``kappa``
    Mobile ion-accessibility function map (as read by :ref:`read` ``kappa``); this causes the :ref:`swin` and :ref:`elecsrfm` parameters and the radii of the biomolecular atoms to be ignored when computing mobile ion values for the Poisson-Boltzmann equation.
    The :ref:`ion` parameter is not ignored and will still be used.
  ``charge``
    Charge distribution map (as read by :ref:`read` ``charge``); this causes the :ref:`chgm` parameter and the charges of the biomolecular atoms to be ignored when assembling the fixed charge distribution for the Poisson-Boltzmann equation.
  ``pot``
    Potential map (as read by :ref:`read` ``pot``); this option requires setting :ref:`bcfl` to ``map``.

``id``
  As described in the READ command documentation (see :ref:`read`), this integer ID specifies the particular map read in with READ.
  These IDs are assigned sequentially, starting from 1, and incremented independently for each map type read by APBS.
  In other words, a calculation that uses two PQR files, one parameter file, three charge maps, and four dielectric maps would have PQR files with IDs 1-2, a parameter file with ID 1, charge maps with IDs 1-3, and dielectric maps with IDs 1-4.

