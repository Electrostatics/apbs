.. _maxvert:

maxvert
=======

.. currentmodule::  apbs.input_file.calculate.finite_element

.. note::  This command has been ported to the *new APBS syntax* (see :ref:`new_input_format`); see :func:`FiniteElement.maximum_vertices`. 

Specify the maximum number of vertices to allow during solve-estimate-refine cycle of finite element solver (:ref:`femanual`).
This places a limit on the memory that can be used by the solver.
The syntax is:

.. code-block:: bash
   
   maxvert { num }

where ``num`` is an integer indicating the maximum number of vertices.
