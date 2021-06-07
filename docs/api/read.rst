.. _read_input_file:

==========================================
Data loading input file section (required)
==========================================


This required section is denoted by the keyword ``read`` and includes objects indexed by the following keywords:

* ``molecules``:  a list of molecule input objects (see :class:`apbs.input_file.read.Molecule`)
* ``potential maps``:  a list of electrostatic potential map input objects (see :class:`apbs.input_file.read.Map`)
* ``charge density maps``:  a list of charge density map input objects (see :class:`apbs.input_file.read.Map`)
* ``ion accessibility maps``:  a list of ion accessibility map input objects (see :class:`apbs.input_file.read.Map`)
* ``dielectric maps``:  a list of dielectric map input objects (see :class:`apbs.input_file.read.DielectricMapGroup`)
* ``parameters``:  a list of parameter files

The syntax for these objects is described below.

----------------------------------------------------------------

.. automodule::  apbs.input_file.read
   :members:
   :undoc-members:

