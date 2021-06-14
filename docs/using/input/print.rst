.. _print:

PRINT input file section
========================

This is a very simple section that allows linear combinations of calculated properties to be written to standard output.
The syntax of this section is:

.. code-block:: bash

    PRINT {what} [id op id op...] END

The first mandatory argument is ``what``, the quantity to manipulate or print.
This variable is a string that can assume the following values:

``elecEnergy``
  Print electrostatic energies as calculated with an earlier :ref:`elec` :ref:`calcenergy` command.
``elecForce``
  Print electrostatic forces as calculated with an earlier :ref:`elec` :ref:`calcforce` command.
``apolEnergy``
  Print apolar energies as calculated with an earlier :ref:`apolar` :ref:`calcenergy` command.
``apolForce``
  Print electrostatic forces as calculated with an earlier :ref:`apolar` :ref:`calcforce` command.

The next arguments are a series of ``id op id op id op ... id`` commands where every ``id`` is immediately followed by an ``op`` and another ``id``.

``id``
  This is a variable string or integer denoting the ID of a particular :ref:`elec` or :ref:`apolar` calculations.
  String values of ``id`` correspond to the optional "names" that can be assigned to :ref:`elec` or :ref:`apolar` calculations.
  Integer values of id are assumed to corresponding to the sequentially-assigned integer IDs for :ref:`elec` or :ref:`apolar` calculations.
  These IDs start at 1 and are incremented (independently) for each new :ref:`elec` or :ref:`apolar` calculation.
``op``
  Specify the arithmetic operation (``+`` for addition and ``-`` for subtraction) to be performed on the calculated quantities

For example:

.. code-block:: python

   # Energy change due to binding
   print energy complex - ligand - protein end
   # Energy change due to solvation
   print energy solvated - reference end
   # Solvation energy change due to binding
   print energy complex_solv - complex_ref - ligand_solv + ligand_ref - protein_solv + protein_ref end

