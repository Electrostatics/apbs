.. _write:

write
=====

This controls the output of scalar data calculated during the Poisson-Boltzmann run.
This keyword can be repeated several times to provide various types of data output from APBS.
The syntax is:

.. code-block:: bash

   write {type} {format} {stem}

``type``
  A string indicating what type of data to output:</p>

  ``charge``
    Write out the biomolecular charge distribution in units of e\ :sub:`c` (electron charge) per Å\ :sup:`3` (multigrid only).
  ``pot``
    Write out the electrostatic potential over the entire problem domain in units of k\ :sub:`b` T e\ :sub:`c`\ :sup:`-1` (multigrid and finite element), where

    k\ :sub:`b`
      Boltzmann's constant:  1.3806504 × 10\ :sup:`−23` J K\ :sup:`-1`

    T
      The temperature of your calculation in K

    e\ :sub:`c`
      is the charge of an electron:  1.60217646 × 10\ :sup:`-19` C

    As an example, if you ran your calculation at 300 K, then the potential would be written out as multiples of
    k\ :sub:`b` T e\ :sub:`c`\ :sup:`-1` = (1.3806504 × 10\ :sup:`−23` J K\ :sup:`-1`) × (300 K) × (1.60217646 × 10\ :sup:`-19` C)\ :sup:`-1` = (4.1419512 × 10\ :sup:`-21` J) × (6.241509752 × 10\ :sup:`18` C\ :sup:`-1`) = 25.85202 mV

  ``atompot``
    Write out the electrostatic potential at each atom location in units of k\ :sub:`b` T e\ :sub:`c`\ :sup:`-1` (multigrid and finite element).
  ``smol``
    Write out the solvent accessibility defined by the molecular surface definition (see :ref:`elecsrfm` ``smol``).
    Values are unitless and range from 0 (inaccessible) to 1 (accessible). (multigrid and finite element).
  ``sspl``
    Write out the spline-based solvent accessibility (see :ref:`elecsrfm` ``spl2``).
    Values are unitless and range from 0 (inaccessible) to 1 (accessible) (multigrid and finite element)
  ``vdw``
    Write out the van der Waals-based solvent accessibility (see :ref:`elecsrfm` ``smol`` with :ref:`srad` 0.0).
    Values are unitless and range from 0 (inaccessible) to 1 (accessible). (multigrid and finite element)
  ``ivdw``
    Write out the inflated van der Waals-based ion accessibility (see :ref:`elecsrfm` ``smol``).
    Values are unitless and range from 0 (inaccessible) to 1 (accessible). (multigrid and finite element)
  ``lap``
    Write out the Laplacian of the potential :math:`\nabla^2 \phi` in units of k\ :sub:`B` T e\ :sub:`c`\ :sup:`-1` Å\ :sup:`-2`  (multigrid only).
  ``edens``
    Write out the "energy density" :math:`-\nabla \cdot \epsilon \nabla \phi` in units of k\ :sub:`B` T e\ :sub:`c`\ :sup:`-1` Å\ :sup:`-2`  (multigrid only).
  ``ndens``
    Write out the total mobile ion number density for all ion species in units of M (multigrid only).
    The output is calculated according to the formula (for nonlinear PB calculations):  :math:`\rho(x) = \sum_i^N {\bar{\rho}_i e^{-q_i\phi(x) - V_i (x)}}`, where *N* is the number of ion species, :math:`\bar{\rho}_i` is the bulk density of ion species *i*, :math:`q_i` is the charge of ion species *i*, :math:`\phi(x)` is the electrostatic potential, and :math:`V_i` is the solute-ion interaction potential for species *i*.
  ``qdens``
    Write out the total mobile ion charge density for all ion species in units of e\ :sub:`c` M (multigrid only).
    The output is calculated according to the formula (for nonlinear PB calculations):  :math:`\rho(x) = \sum_i^N {\bar{\rho}_i q_i e^{-q_i\phi(x) - V_i (x)}}`, where *N* is the number of ion species, :math:`\bar{\rho}_i` is the bulk density of ion species *i*, :math:`q_i` is the charge of ion species *i*, :math:`\phi(x)` is the electrostatic potential, and :math:`V_i` is the solute-ion interaction potential for species *i*.
  ``dielx`` or ``diely`` or ``dielz``
    Write out the dielectric map shifted by 1/2 grid spacing in the {x, y, z}-direction (see :ref:`read` ``diel``).
    The values are unitless (multigrid only).

``format``
  A string that specifies the format for writing out the data:

  ``dx``
    Write out data in :doc:`/formats/opendx`.
    This is the preferred format for APBS I/O. (multigrid and finite element).

  ``avs``
    Write out data in AVS UCD format. (finite element only).

  ``uhbd``
    Write out data in :doc:`/formats/uhbd`. (multigrid only).

  ``gz``
    Write out :doc:`/formats/opendx` in gzipped (zlib) compatible format.
    Appends .dx.gz to the filename.

  ``flat``
    Write out data as a plain text file. (multigrid and finite element).

``stem``
  A string that specifies the path for the output; files are written to :file:`stem.{XYZ}`, where ``XYZ`` is determined by the file format (and processor rank for parallel calculations).
  If the pathname contains spaces, then it must be surrounded by double quotes.
