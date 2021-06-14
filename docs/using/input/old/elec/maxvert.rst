.. _maxvert:

maxvert
=======

Specify the maximum number of vertices to allow during solve-estimate-refine cycle of finite element solver (:ref:`femanual`).
This places a limit on the memory that can be used by the solver.
The syntax is:

.. code-block:: bash
   
   maxvert { num }

where ``num`` is an integer indicating the maximum number of vertices.
