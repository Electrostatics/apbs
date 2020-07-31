.. _akeySOLVE:

akeySOLVE
=========

Specifies how the the finite element mesh should be adaptively subdivided during the solve-estimate-refine iterations of a :ref:`femanual` finite element calculation.
The syntax is:

.. code-block:: bash

   akeySOLVE {key}

where ``key`` is a text string that specifies the method used to guide adaptive refinement:

``resi``
  Residual-based a *posteriori* refinement.

