.. _apbsflatparm:

APBS flat-file parameter format
===============================

This parameter file format is a series of lines of the form:

.. code-block:: bash

   Residue_name Atom_name Charge Radius Epsilon

where the whitespaces are important and denote separation between the fields.
The fields here are:

``Residue_name``
  A string giving the residue name, as provided in the PDB file to be parametrized.

``Atom_name``
  A string giving the atom name, as provided in the PDB file to be parametrized.

``Charge``
  A float giving the atomic charge (in electrons).

``Radius``
  A float giving the atomic radius (in Å).

``Epsilon``
  A float giving the Lennard-Jones well depth (epsilon, in kJ/mol).
  This is used for the calculation of WCA energies in apolar solvation energies and forces.
  We assume that the Lennard-Jones potential is defined in the "AMBER style"
