.. _read_new_input:

==========================================
Data loading input file section (required)
==========================================


This required section is denoted by the keyword ``read`` and includes objects indexed by the following keywords:

* ``molecules``:  a list of molecule input objects
* ``potential maps``:  a list of electrostatic potential map input objects
* ``charge density maps``:  a list of charge density map input objects
* ``ion accessibility maps``:  a list of ion accessibility map input objects
* ``dielectric maps``:  a list of dielectric map input objects
* ``parameters``:  a list of parameter files

The syntax for these objects is described in :class:`apbs.input_file.read.Read`.

See also:

.. toctree::
   :maxdepth: 2

   read_api