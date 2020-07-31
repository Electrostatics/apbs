.. _async:

async
=====

An optional keyword to perform an asynchronous parallel focusing Poisson-Boltzmann equation.
The syntax is

.. code-block:: bash

   async {rank}

where ``rank`` is the integer ID of the particular processor to masquerade as.
Processor IDs range from *0* to *N-1*, where *N* is the total number of processors in the run (see :ref:`pdime`).
Processor IDs are related to their position in the overall grid by :math:`p = nx ny k + nx j + i`  where :math:`nx` is the number of processors in the x-direction, :math:`ny` is the number of processors in the y-direction, :math:`nz` is the number of processors in the z-direction, :math:`i` is the index of the processor in the x-direction, :math:`j` is the index of the processor in the y-direction, :math:`k` is the index of the processor in the z-direction, and :math:`p` is the overall rank of the processor.

