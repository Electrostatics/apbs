.. _apolarsrfm:

srfm (apolar)
=============

.. currentmodule:: apbs.input_file.calculate.nonpolar

.. note::  

   This command has been ported to the *new APBS syntax* (see :ref:`new_input_format`):  see :func:`Nonpolar.surface_method` for more information.

This keyword specifies the model used to construct the solvent-related surface and volume.
The syntax is:

.. code-block:: bash

   srfm {flag}

where ``flag`` is a string that specifies the model used for surface and volume.
Acceptable values of flag include:

``sacc``
  Solvent-accessible (also called "probe-inflated") surface and volume.

  
