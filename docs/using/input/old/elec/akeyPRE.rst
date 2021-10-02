.. _akeyPRE:

akeyPRE
=======

.. currentmodule:: apbs.input_file.calculate.finite_element

.. note::  This command has been ported to :func:`FiniteElement.a_priori_refinement`.

Specifies how the initial finite element mesh should be constructed (from refinement of a very coarse 8-tetrahedron mesh prior to the solve-estimate-refine iteration in :ref:`femanual` finite element calculations.
The syntax is:

.. code-block:: bash

   akeyPRE {key}

where ``key`` is a text string that specifies the method used to guide initial refinement and takes one of the values:

``unif``
  Uniform refinement
``geom``
  Geometry-based refinement at molecular surfaces and charges

