.. _ekey:

ekey
====

Specify the method used to determine the error tolerance in the solve-estimate-refine iterations of the finite element solver (:ref:`femanual`).
The syntax is:

.. code-block:: bash

   ekey { flag }

where ``flag`` is a text string that determines the method for error calculation.

``simp``
  Per-simplex error limit
``global``
  Global (whole domain) error limit
``frac``
  Fraction of simplices you'd like to see refined at each iteration
