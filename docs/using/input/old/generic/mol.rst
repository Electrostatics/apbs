.. _mol:

mol
===

.. note::  

   Some versions of this command have been ported to the *new APBS syntax* (see :ref:`new_input_format`):

   .. currentmodule::  apbs.input_file.calculate

   * Nonpolar calculations: see :func:`nonpolar.Nonpolar.molecule` for more information.

   * Finite difference Poisson-Boltzmann calculations: see :func:`finite_difference.FiniteDifference.molecule` for more information.

.. todo:: port for other calculation types

This term specifies the molecule for which the calculation is to be performed.
The syntax is:

.. code-block:: bash
   
   mol {id}
   

where ``id`` is the integer ID of the molecule for which the apolar calculation is to be performed.
The molecule IDs are based on the order in which molecules are read by ``READ mol`` statements (see :ref:`read_old_input`), starting from 1.
