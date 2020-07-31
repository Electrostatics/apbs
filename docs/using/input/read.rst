.. _read:

READ input file section
=======================

The READ block of an APBS input file has the following general format:

.. code-block:: bash
   
   READ
       [ keywords... ]
   END

where ``keywords`` is or more of the keywords described below (the line breaks and indentation are for clarity; only whitespace is necessary).

.. note::
   One of these sections must be present for every molecule involved in the APBS calculation.
   Molecule and "map" IDs are assigned implicitly assigned for each molecule/map read, based on order and starting at 1 and incremented independently for each input type.
   In other words, each input PQR file is assigned an ID 1, 2, 3, ...; each input dielectric map is assigned an independent ID 1, 2, 3, ...; etc.

------
charge
------

This command allows APBS to read the fixed (molecular) charge density function mapped to a mesh.
The inputs are maps of charge densities; these values have units of e\ :sub:`c` Å\ :sup:`-3`, where e\ :sub:`c` is the electron charge.
In general, this command will read charge-maps written by :ref:`elec` :ref:`write` commands.
The syntax of this command is:

.. code-block:: bash

   READ charge {format} {path} END 

``format``
  Specify the format of the charge map.
  Acceptable values include:

  ``dx``
    :ref:`opendx`

  ``gz``
    gzipped (zlib) compressed :ref:`opendx`.
    Files can be read directly in compressed form.

``path``
  The location of the charge map file.


----
diel
----

This command allows APBS to read the dielectric function mapped to 3 meshes shifted by one-half grid spacing in the x, y, and z directions.
The inputs are maps of dielectric variables between the solvent and biomolecular dielectric constants; these values are unitless.
In general, this command will read dielectric maps written by by :ref:`elec` :ref:`write` commands.
The syntax of this command is:

.. code-block:: bash

   READ diel {format} {path-x} {path-y} {path-z} END

``format``
  The format of the dielectric map.

  ``dx``
    :ref:`opendx`
  
  ``gz``
    gzipped (zlib) compressed :ref:`opendx`.
    Files can be read directly in compressed form.

``path-x``
  The location of the x-shifted dielectric map file.

``path-y``
  The location of the y-shifted dielectric map file.

``path-z`` The location of the z-shifted dielectric map file.

.. note::

   If you choose this option and have a non-zero ionic strength, you must also include a READ kappa_ statement.

-----
kappa
-----

This command allows APBS to read the ion-accessibility function mapped to a mesh.
The inputs are maps of ion accessibility values which range between 0 and the build Debye-Hückel screening parameter; these values have units of Å\ :sup:`-2`.
In general, this command will read kappa-maps written by by :ref:`elec` :ref:`write` commands.
The syntax of this command is:

.. code-block:: bash

   READ kappa {format} {path} END

``format``
  Specify the format of the charge map.
  Acceptable values include:

  ``dx``
    :ref:`opendx`
  
  ``gz``
    gzipped (zlib) compressed :ref:`opendx`.
    Files can be read directly in compressed form.

``path``
  The location of the map file.


.. note::

   If you choose this option, you must also include a read diel statement.

---
mol
---

This command specifies the molecular data to be read into APBS.
The syntax is

.. code-block:: bash

   READ mol {format} {path} END

``format``
  The format of the input data.

  ``pqr``
    Specify that molecular data is in :ref:`PQR format <pqr>`.
  
  ``pdb``
    Specify that molecular data is in pseudo-PDB format.
    If this type of structure file is used, then a parameter file must also be specified with a READ parm_ statement to provide charge and radius parameters for the biomolecule's atoms.

``path``
  The location of the molecular data file.

----
parm
----

This command specifies the charge and radius data to be used with pseudo-PDB-format molecule files.
The syntax is:

.. code-block:: bash

   READ parm {format} {path} END

``format``
  The format of the parameter file.

  ``flat``
    Specify that the parameter file is in :ref:`APBS flat-file parameter format <apbsflatparm>`.

  ``xml``
    Specify that the parameter file is in :ref:`APBS XML parameter format <apbsxmlparm>`

``path``
  The location of the parameter data file.

.. note::
   
   APBS provides a few example files as part of the source code distribution.
   Currently, example files only contain the polar parameters that can also be assigned more easily through the PDB2PQR software.

---
pot
---

This command allows APBS to read the electrostatic potential mapped to a mesh.
The inputs are maps of the electrostatic potential from a previous calculation.
In general, this command will read potential-maps written by by :ref:`elec` :ref:`write` commands.
The syntax of this command is:

.. code-block:: bash

   READ pot {format} {path} END

``format``
  Specify the format of the charge map.
  Acceptable values include:

  ``dx``
    :ref:`opendx`

  ``gz``
    gzipped (zlib) compressed :ref:`opendx`.
    Files can be read directly in compressed form.

``path``
  The location of the map file.

.. note::
   
   To use this functionality you must set the :ref:`bcfl` keyword to ``map``.
   See also: :ref:`usemap`.
