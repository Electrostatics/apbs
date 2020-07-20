.. _term:

term
====

Specify a termination condition for a PB-(S)AM Brownian dynamics trajectory.
The syntax is:

.. code-block:: bash
   
   term {type} {options}

where the ``options`` are determined by the ``type`` as follows:

``contact {file}``
  Termination based on molecular contact conditions.
  ``file`` is a string for the contact file filename.
  The contact file has a list formatted as follows:  ``moltype1 at1 moltype2 at2 dist`` where ``moltype1``  and ``moltype2``  are indices of the molecular types, ``at1`` is the index of an atom from the first molecular type, ``at2`` is the index of an atom from the second molecular type, and ``dist`` is the maximum distance between the two atoms that defines the contact.
  ``pad`` is distance criterion that will be checked in the case that the true atom contact distance may not be fulfilled.

  .. note::

     Sometimes these distances cannot be reached due to the assumption in this model that the molecule is spherical.
     If this is the case, the atom positions are transformed to the molecule surface and surface points are compared to the pad distance.

``{pos} {val} {molecule}``
  Specify a position termination condition for a given molecule.
  where ``pos`` is one of the following options: ``x<=, x>=, y<=, y>=, z<=, z>=, r<=, r>=``.
  ``val`` is the value along the given axis to check against.
  ``molecule`` is the molecule index (1 based) according to the order of molecules listed in the ``READ`` section that this condition applies to.
  This command can be understood as:  "Terminate the simulation when molecule ``molecule`` fulfills the condition ``pos`` ``val``".

  .. todo::

     Add a constant keyword (e.g., like ``position``) before the ``{pos}`` argument of ``term``.
     Documented in https://github.com/Electrostatics/apbs-pdb2pqr/issues/503

``time {val}``
  Specify a time termination condition where ``val`` is a floating point number for the trajectory time limit (in picoseconds).
