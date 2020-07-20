.. _exp:

exp
===

This keyword can be used to load in the expansion matrices from files.
They will have been previously generated, and will be named :file:`mol{m}.{H, F}.[s].exp` (see :ref:`pbamauto` for more information).
The syntax is:

.. code-block:: bash
   
   exp {prefix}

where ``prefix`` is the filename prefix :file:`mol{m}sph`.
The *H* or *F* and :file:`{s}.bin` will be appended during the program run.

.. todo::

   It would be better to generalize the :ref:`read` section of the input file rather than use the ``exp`` command.
   This command also needs to be cleaned up -- it's too fragile.
   Documented at https://github.com/Electrostatics/apbs-pdb2pqr/issues/489