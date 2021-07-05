.. _akeySOLVE:

akeySOLVE
=========

.. note::  This command has been eliminated in the *new APBS syntax* (see :ref:`new_input_format`) and will not be ported.

Specifies how the the finite element mesh should be adaptively subdivided during the solve-estimate-refine iterations of a :ref:`femanual` finite element calculation.
The syntax is:

.. code-block:: bash

   akeySOLVE {key}

where ``key`` is a text string that specifies the method used to guide adaptive refinement:

``resi``
  Residual-based a *posteriori* refinement.

