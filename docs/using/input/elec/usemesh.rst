.. _usemesh:

usemesh
=======

Specify the external finite element mesh to be used in the finite element Poisson-Boltzmann calculation (:ref:`femanual`).
These must have been input via an earlier READ mesh statement (see :ref:`read`).
The syntax is:

.. code-block:: bash

   usemesh {id}

where ``id`` is an integer ID specifying the particular map read in with :ref:`read`.
These IDs are assigned sequentially, starting from 1, and incremented independently for each mesh read by APBS.
