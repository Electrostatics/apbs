.. _tree_order:

tree_order
==========

.. todo::  This command has not yet been ported to the *new APBS syntax* (see :ref:`new_input_format`).

TABI-PB parameter that specifies the order of the treecode multipole expansion.
The syntax is:

.. code-block:: bash

   tree_order {order}

where ``order`` is an integer that indicates the Taylor expansion order.
Users can adjust the order for different accuracy. 
A typical choice for this parameter is 3.
