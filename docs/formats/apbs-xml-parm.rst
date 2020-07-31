.. _apbsxmlparm:

APBS XML parameter format
=========================

This parameter file format has the following form:

.. code-block:: xml

   <ffname>
      <residue>
          <name>resname</name>
          <atom>
              <name>atomname</name>
              <charge>atomcharge</charge>
              <radius>atomradius</radius>
              <epsilon>atomepsilon</epsilon>
          </atom>
          ...
      </residue>
      ...
   </ffname>

The variables in this example are:

``ffname``
  The name of the forcefield. This is the root element of the XML file.

``resname``
  A string giving the residue name, as provided in the PDB file to be parameterized.

``atomname``
  A string giving the atom name, as provided in the PDB file to be parameterized.

``atomcharge``
  A float giving the atomic charge (in electrons).

``atomradius``
  A float giving the atomic Radius (in Å).

``atomepsilon``
  A float giving the Lennard-Jones well depth :math:`\epsilon` (in kJ/mol).
  This is used for the calculation of WCA energies in apolar solvation energies and forces.
  We assume that the Lennard-Jones potential is defined in the "AMBER style"