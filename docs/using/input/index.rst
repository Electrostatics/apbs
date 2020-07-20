================
APBS input files
================

APBS input files are loosely-formatted files which contain information about the input, parameters, and output for each calculation.

These files are whitespace- or linefeed-delimited.
Comments can be added to the input files via the ``#`` character; all text between the ``#`` and the end of the line is not parsed by APBS.
If pathnames used in the input file contain spaces, then the entire pathname must be enclosed in quotes.
For example, if you wanted to refer to the file :file:`foo` which resides in a directory with spaces in its name, then you should refer to :file:`foo` as :file:`"/path with spaces/foo"`.
Specific examples of APBS input are provided in :doc:`/examples/index`.

APBS input files contain three basic sections which can be repeated any number of times:

:ref:`read`
  section for specifying input
:ref:`elec`
  section for specifying polar solvation (electrostatics) calculation parameters
:ref:`apolar`
  section for specifying apolar solvation calculation parameters
:ref:`print`
  section for specifying summary output

The APBS input file is constructed from these sections in the following format:

.. code-block:: bash
   
   READ
   ...
   END
   
   ELEC
   ...
   END
   
   APOLAR
   ...
   END
   
   PRINT
   ...
   END
   
   QUIT

These sections can occur in any order and can be repeated any number of times.
However, the sections are inter-dependent.
For example, PRINT requires ELEC and/or APOLAR while ELEC requires one or more READ sections.
Sections can also be repeated; several READ statements may be used to load molecules and multiple ELEC or APOLAR sections would specify various electrostatics calculations on one or more molecules.

Each section has the following syntax:

.. code-block:: bash
   
   SECTION [name <id>]

where the optional ``name`` argument allows the user to include a string to identify the section.
In the absence of this argument, sections are assigned numerical IDs.

.. toctree::
   :maxdepth: 2
   :caption: Contents:

   read
   elec/index
   apolar/index
   print
