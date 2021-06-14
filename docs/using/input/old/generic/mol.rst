.. _mol:

mol
===

This term specifies the molecule for which the calculation is to be performed.
The syntax is:

.. code-block:: bash
   
   mol {id}
   

where ``id`` is the integer ID of the molecule for which the apolar calculation is to be performed.
The molecule IDs are based on the order in which molecules are read by ``READ mol`` statements (see :ref:`read`), starting from 1.
