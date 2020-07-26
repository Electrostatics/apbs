.. _mesh:

mesh
====

TABI-PB parameter that spceifies the meshing software used to generate surface mesh.
The syntax is:

.. code-block:: bash

   mesh {flag}

where ``flag`` is an integer indicating the meshing software to be used:

.. _MSMS: http://mgl.scripps.edu/people/sanner/html/msms_home.html
.. _NanoShaper: https://www.electrostaticszone.eu/downloads

0
  MSMS_
1
  SES implementation in NanoShaper_
2
  Skin surface implementation in NanoShaper_

The default avlue is MSMS_.
Note that the executables for MSMS_ and NanoShaper_ must be included in your path to use them.

.. todo::

   The integer flag values for ``mesh`` should be replaced by human-readable strings.
   Documented in https://github.com/Electrostatics/apbs-pdb2pqr/issues/496
   