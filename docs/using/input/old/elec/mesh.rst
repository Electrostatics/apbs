.. _mesh:

mesh
====

.. currentmodule::  apbs.input_file.calculate.boundary_element

.. note::

   This command has been ported to the *new APBS syntax* (see :ref:`new_input_format`); see :func:`Mesh.surface_method`.


TABI-PB parameter that specifies the meshing software used to generate surface mesh.
The syntax is:

.. code-block:: bash

   mesh {flag}

where ``flag`` is an integer indicating the meshing software to be used:

.. _NanoShaper: https://www.electrostaticszone.eu/downloads

0
  Formerly used for msms, no longer supported.
1
  SES implementation in NanoShaper_
2
  Skin surface implementation in NanoShaper_

Note that the executable NanoShaper_ must be included in your path to use them.

.. todo::

   The integer flag values for ``mesh`` should be replaced by human-readable strings.
   Documented in https://github.com/Electrostatics/apbs/issues/496
   