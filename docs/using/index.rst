==========
Using APBS
==========

.. _PDB ID: https://www.rcsb.org/pages/help/advancedsearch/pdbIDs

.. note::

   *Before you begin!* PDB2PQR funding is dependent on your help for continued development and support. Please `register <http://eepurl.com/by4eQr>`_ before using the software so we can accurately report the number of users to our funding agencies.

APBS is often used together with the `PDB2PQR software <https://github.com/Electrostatics/pdb2pqr>`_; e.g., ,in the following type of workflow

#. Start with a `PDB ID`_ or locally generated PDB file (see :doc:`/formats/pdb`).
#. Assign titration states and parameters with :program:`pdb2pqr` to convert the protein and ligands to PQR format (see :doc:`/formats/pqr`).
#. Perform electrostatics calculations with :program:`apbs` (can be done from within the `PDB2PQR web server <web-server>`_).
#. Visualize results from within PDB2PQR web server or with :doc:`other-software`.

--------------
Web server use
--------------

Most users will use PDB2PQR through `the web server <http://server.poissonboltzmann.org/>`_ (after `registering <http://eepurl.com/by4eQr>`_, of course).
However, it is also possible to install local versions of PDB2PQR and run these through the command line.

----------------
Command line use
----------------

.. code-block:: bash

   apbs [options] input-file

where the list of ``[options]`` can be obtained by running APBS with the ``--help`` option.
The input file format is described in :doc:`input/index`.

-------------------
Tools and utilities
-------------------

.. toctree::
   :maxdepth: 2

   tools

----------------
More information
----------------

.. toctree::
   :maxdepth: 2

   input/index.rst
   tools
   other-software