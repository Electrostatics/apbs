.. _tree_order:

tree_order
==========

.. currentmodule::  apbs.input_file.calculate.boundary_element

.. note::

   This command has been ported to the *new APBS syntax* (see :ref:`new_input_format`); see :func:`TABIParameters.tree_order`.

TABI-PB parameter that specifies the order of the treecode multipole expansion.
The syntax is:

.. code-block:: bash

   tree_order {order}

where ``order`` is an integer that indicates the Taylor expansion order.
Users can adjust the order for different accuracy. 
A typical choice for this parameter is 3.
