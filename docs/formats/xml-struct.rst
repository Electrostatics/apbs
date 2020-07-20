.. _xmlstruct:

XML molecular structure format
==============================

The XML structure format was designed as a light-weight alternative to remediate some of the shortcomings of the flat-file format.
By use of XML, issues related to extra fields in the file or columns merging together can easily be remedied.
Additionally, APBS will only parse the necessary information from the XML file and will ignore all other information, so users wishing to store extra data related to a residue or atom can do so inline without affecting APBS.

This data format has the following form:

.. code-block:: xml

   <roottag>
      <residue>
          <atom>
              <x>x</x>
              <y>y</y>
              <z>z</z>
              <charge>charge</charge>
              <radius>radius</radius>
          </atom>
          ...
      </residue>
      ...
   </roottag>

The variables in this example are:

``roottag``
  This is the root element of the XML file. The value is not important to APBS - APBS simply checks that it is closed at the end of the file.

``x y z``
  A float giving the {x, y, z}-coordinate of the atom in Å.

``charge``
  A float giving the atomic charge (in electrons).

``atomradius``
  A float giving the atomic Radius (in Å).

.. note::

   Yes, we probably should have used `PDBML <http://pdbml.pdb.org/>`_ instead.
