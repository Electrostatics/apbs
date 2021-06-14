.. _sdens:

sdens
=====

This keyword specifies the number of quadrature points per Å\ :superscript:`2` to use in calculation surface terms (e.g., molecular surface, solvent accessible surface).
This keyword is ignored when :ref:`srad` is 0.0 (e.g., for van der Waals surfaces) or when :ref:`elecsrfm` is ``spl2`` (e.g., for spline surfaces).
The syntax is:

.. code-block:: bash

   sdens {density}

where ``density`` is a floating point number indicating the number of grid points per Å\ :superscript:`-2`.
A typical value is 10.0.

.. note::
   There is a strong correlation between the value used for the sphere density, the accuracy of the results, and the APBS calculation time.
