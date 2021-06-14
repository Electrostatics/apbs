.. _ion:

ion
===

Specify the bulk concentrations of mobile ion species present in the system.
This command can be repeated as necessary to specify multiple types of ions; however, only the largest ionic radius is used to determine the ion-accessibility function.
The total bulk system of ions must be electroneutral which means the charge densities/concentrations of positive and negative ions must be equal.
The syntax is:

.. code-block:: bash

   ion charge {charge} conc {conc} radius {radius}

where

``charge``
  Mobile ion species charge (floating point number in ec)
``conc``
  Mobile ion species concentration (floating point number in M)
``radius``
  Mobile ion species radius (floating point number in Å)

