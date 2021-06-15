.. _exp:

exp
===

.. todo::  This command has not yet been ported to the *new APBS syntax* (see :ref:`new_input_format`).

This keyword can be used to load in the expansion matrices from files.
They will have been previously generated, and will be named :file:`mol{m}.{H, F}.[s].exp` (see :ref:`pbamauto` for more information).
The syntax is:

.. code-block:: bash
   
   exp {prefix}

where ``prefix`` is the filename prefix :file:`mol{m}sph`.
The *H* or *F* and :file:`{s}.bin` will be appended during the program run.

.. todo::

   It would be better to generalize the :ref:`read_old_input` section of the input file rather than use the ``exp`` command.
   This command also needs to be cleaned up -- it's too fragile.
   Documented at https://github.com/Electrostatics/apbs/issues/489