.. _read_new_input:

==========================================
Data loading input file section (required)
==========================================

.. currentmodule:: apbs.input_file.read

This required section is denoted by the keyword ``read`` and is described in :class:`Read`.
The section includes objects indexed by the following keywords:

* ``molecules``:  a list of molecule input objects of class :class:`Molecule`
* ``potential maps``:  a list of electrostatic potential map input objects of class :class:`Map`
* ``charge density maps``:  a list of charge density map input objects of class :class:`Map`
* ``ion accessibility maps``:  a list of ion accessibility map input objects of class :class:`Map`
* ``dielectric maps``:  a list of dielectric map input objects of class :class:`DielectricMapGroup`
* ``parameters``:  a list of parameter files of class :class:`Parameter`

--------------------------------------------------------------------------------

---------------------------
Data loading input file API
---------------------------

.. automodule::  apbs.input_file.read
   :members:
   :undoc-members:
   :inherited-members:
   :show-inheritance:
