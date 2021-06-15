.. _process_new_input:

=====================================
Results processing section (optional)
=====================================


This optional section is denoted by the keyword ``process`` and includes lists of arithmetic operation objects (see :class:`apbs.input_file.process.Operation`) indexed by the following keywords:

* ``sums``:  a list weighted sum operations
* ``products``:  a list of weighted product operations
* ``exps``:  a list of element-wise exponentiation operations

The syntax for these objects is described in :class:`apbs.input_file.process.Process`.

See also:

.. toctree::
   :maxdepth: 2

   process_api
