.. _maxsolve:

maxsolve
========

.. todo::  This command has not yet been ported to the *new APBS syntax* (see :ref:`new_input_format`).

Specify the number of times to perform the solve-estimate-refine iteration of the finite element solver (:ref:`femanual`).
The syntax is:

.. code-block:: bash
   
   maxsolve { num }

where `num` is an integer indicating the desired maximum number of iterations.
