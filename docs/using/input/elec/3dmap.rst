.. _3dmap:

3dmap
=====

Specify the name of the file into which the potential surface on the coarse-grain molecule surface will be printed.

.. code-block:: bash
   
   3dmap {filename}

where ``filename`` is a string for the name of the file where a 3D grid will be printed out.

.. todo::
   
   The PB-(S)AM ``3dmap`` keyword should not exist; please replace it ASAP with the :ref:`write` command.
   Documented this todo as https://github.com/Electrostatics/apbs-pdb2pqr/issues/482
