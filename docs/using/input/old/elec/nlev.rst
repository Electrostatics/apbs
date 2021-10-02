.. _nlev:

nlev
====

.. note::

   ..currentmodule:: apbs.input_file.calculate.finite_difference

   This command has been eliminated in the *new APBS syntax* (see :ref:`new_input_format`); see :class:`GridDimensions` for more information.


Specify the depth of the multilevel hierarchy used in the :ref:`mgmanual` multigrid solver.
See :ref:`dime` for a discussion of how nlev relates to grid dimensions.
The syntax is:

.. code-block:: bash
   
   nlev {lev}

where ``lev`` is an integer indicating the desired depth of the multigrid hierarchy.

