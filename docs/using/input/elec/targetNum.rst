.. _targetNum:

targetNum
=========

Specify the target number of vertices in the initial finite element mesh for :ref:`femanual` calculations.
The syntax is:

.. code-block:: bash

   targetNum { num }

where ``num`` is an integer denoting the target number of vertices in initial mesh.
Initial refinement will continue until this number is reached or the the longest edge of every simplex is below :ref:`targetRes`.
