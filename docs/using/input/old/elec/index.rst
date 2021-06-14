.. _elec:

ELEC input file section
=======================

The ELEC block of an APBS input file is used for polar solvation (electrostatics) calculations and has the following syntax:

.. code-block:: bash
   
   ELEC [ name {id} ]
        {type}
        {keywords...}
   END

The optional ``id`` variable is a simple string that allows ELEC statements to be named.
Since numerous ELEC blocks may appear in an APBS input file, it can be difficult to keep track of them all.
It is possible to assign an optional name (string) to each ELEC block to simplify the organizational process.

The ``type`` command defines the types of ELEC calculation to be performed and includes:

* Finite difference multigrid calculations with `PMG <http://www.fetk.org>`_.

  * :ref:`mgauto`
  * :ref:`mgpara`
  * :ref:`mgmanual`

* `Geometric flow solvation <https://www.ncbi.nlm.nih.gov/pubmed/23212974>`_ finite difference calculations

  * :ref:`geoflowauto`

* Boundary element method calculations with `TABI-PB <https://doi.org/10.1016/j.jcp.2013.03.056>`_.

  * :ref:`tabi`

* Analytic and semi-analytic Poisson-Boltzmann approximations

  * :ref:`pbamauto`
  * :ref:`pbsamauto`

* Finite element calculations with `FEtk <http://www.fetk.org>`_.

  * :ref:`femanual`

* No-op modes for generating coefficient maps

  * :ref:`mgdummy`

Finally, the ``keywords`` are calculation-specific commands that customize the particular type of calculation.
This section is the main component for polar solvation calculations in APBS runs.
There may be several ELEC sections, operating on different molecules or using different parameters for multiple runs on the same molecule.
The order of the ELEC statement can matter since certain types of boundary conditions (:ref:`bcfl`) can require information about previous calculations.


.. toctree::
   :maxdepth: 1
   :caption: Calculation type keywords

   tabi
   fe-manual
   geoflow-auto
   mg-auto
   mg-manual
   mg-para
   mg-dummy
   pbam-auto
   pbsam-auto
