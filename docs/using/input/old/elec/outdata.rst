.. _outdata:

outdata
=======

TABI-PB parameter that specifies the file type for printing the output data.
The syntax is:

.. code-block:: bash
   
   outdata {flag}

where ``flag`` is an integer indicating the output file types:

0
  .dat format
1
  Both the .dat format and a VTK polygonal data file that can be visualized in the ParaView software.
  The VTK file contains color mappable potentials and normal derivatives of potentials on the faces and vertices of the mesh.


.. todo::

   The integer flag values for ``mesh`` should really be replaced by human-readable strings.
