.. _surf:

surf
====

This keyword can be used to load in the MSMS vertex file for coarse-graining (see :ref:`pbsamauto`)
The syntax is:

.. code-block:: bash

   surf {prefix}

where ``prefix`` refers to the filename :file:{prefix}.vert`.

.. todo::
   
   The PB-SAM ``surf`` command is redundant with and should be replaced by the existing :ref:`usemesh` command.
   Documented in https://github.com/Electrostatics/apbs-pdb2pqr/issues/502
